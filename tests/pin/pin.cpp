// Characterization pin. Each scenario runs real production code (main() and
// the classes it uses) against the fakes in shim.h and compares every frame
// the game displays with tests/golden. Usage: pin <scenario> | pin --list.
// Env: PIN_GOLDEN (golden dir), PIN_OUT (where mismatches go), PIN_UPDATE=1
// (rewrite the manifests instead of comparing), PIN_DUMP (absolute dir; also
// save every displayed frame there as <hash prefix>.png).

#include <CommonCrypto/CommonDigest.h>
#include <sys/stat.h>
#include <unistd.h>

#define main petpal_main
#include "../../src/main.cpp"
#undef main

namespace {

const sf::Time kTick = sf::milliseconds(125);
const char* kMain = "My Virtual Pet";

std::string envOr(const char* name, const std::string& fallback) {
  const char* v = std::getenv(name);
  return v && *v ? v : fallback;
}

// SHA-256 of width and height (4 bytes each, little endian) then the raw RGBA
// pixels, as lowercase hex.
std::string hashImage(const sf::Image& img) {
  CC_SHA256_CTX ctx;
  CC_SHA256_Init(&ctx);
  for (unsigned dim : {img.getSize().x, img.getSize().y}) {
    unsigned char le[4] = {static_cast<unsigned char>(dim), static_cast<unsigned char>(dim >> 8),
                           static_cast<unsigned char>(dim >> 16),
                           static_cast<unsigned char>(dim >> 24)};
    CC_SHA256_Update(&ctx, le, 4);
  }
  CC_SHA256_Update(&ctx, img.getPixelsPtr(),
                   static_cast<CC_LONG>(img.getSize().x * img.getSize().y * 4));
  unsigned char digest[CC_SHA256_DIGEST_LENGTH];
  CC_SHA256_Final(digest, &ctx);
  char hex[2 * CC_SHA256_DIGEST_LENGTH + 1];
  for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) std::snprintf(hex + 2 * i, 3, "%02x", digest[i]);
  return hex;
}

struct Frame {
  std::string window, step, hash;
};

class Recorder {
 public:
  Recorder(const std::string& scenario)
      : scenario_(scenario),
        golden_(envOr("PIN_GOLDEN", "tests/golden")),
        out_(envOr("PIN_OUT", "build/pin-out")),
        dump_(envOr("PIN_DUMP", "")),
        update_(envOr("PIN_UPDATE", "0") == "1") {
    std::ifstream in(manifestPath());
    hasManifest_ = in.is_open();
    std::string line;
    while (std::getline(in, line)) {
      std::istringstream row(line);
      std::string index;
      Frame f;
      std::getline(row, index, '\t');
      std::getline(row, f.window, '\t');
      std::getline(row, f.step, '\t');
      std::getline(row, f.hash, '\t');
      expected_.push_back(f);
    }
  }

  std::string step = "start";

  void check(bool ok, const std::string& what) {
    if (!ok) fail(what);
  }

  void fail(const std::string& what) {
    failures_++;
    std::cerr << "PIN FAIL " << scenario_ << ": " << what << "\n";
  }

  void capture(sf::PinWindow& window) {
    sf::Image img = window.getTexture().copyToImage();
    Frame f{window.title(), step, hashImage(img)};
    std::size_t index = actual_.size();
    actual_.push_back(f);
    if (!dump_.empty()) dumpFrame(f.hash, img);
    if (update_) return;
    if (index >= expected_.size()) {
      fail("extra frame " + std::to_string(index) + " [" + f.window + " / " + f.step + "]");
      writeMismatch(index, img);
      return;
    }
    const Frame& want = expected_[index];
    if (want.window != f.window || want.step != f.step) {
      fail("frame " + std::to_string(index) + " is [" + f.window + " / " + f.step +
           "], golden has [" + want.window + " / " + want.step + "]");
    }
    if (want.hash != f.hash) {
      fail("frame " + std::to_string(index) + " [" + f.window + " / " + f.step +
           "] expected " + want.hash + ", got " + f.hash);
      writeMismatch(index, img);
    }
  }

  // Exact-text golden for files the game writes (the save file).
  void checkText(const std::string& name, const std::string& text) {
    std::string path = golden_ + "/" + scenario_ + "." + name;
    if (update_) {
      std::ofstream(path) << text;
      return;
    }
    std::ifstream in(path);
    std::stringstream want;
    want << in.rdbuf();
    if (!in.is_open()) return fail("missing golden " + path);
    if (want.str() != text) {
      fail(name + " differs from " + path);
      std::ofstream(out_ + "/" + scenario_ + "." + name) << text;
    }
  }

  int finish() {
    if (update_) {
      std::ofstream out(manifestPath());
      for (std::size_t i = 0; i < actual_.size(); ++i)
        out << i << '\t' << actual_[i].window << '\t' << actual_[i].step << '\t'
            << actual_[i].hash << '\n';
    } else if (!hasManifest_) {
      fail("missing golden " + manifestPath());
    } else if (actual_.size() < expected_.size()) {
      fail("only " + std::to_string(actual_.size()) + " of " +
           std::to_string(expected_.size()) + " golden frames were displayed");
    }
    std::cerr << "PIN " << (failures_ ? "FAIL " : "ok   ") << scenario_ << " ("
              << actual_.size() << " frames)\n";
    return failures_ ? 1 : 0;
  }

 private:
  std::string manifestPath() const { return golden_ + "/" + scenario_ + ".txt"; }
  void dumpFrame(const std::string& hash, const sf::Image& img) {
    std::string path = dump_ + "/" + hash.substr(0, 16) + ".png";
    if (std::ifstream(path).good()) return;
    std::string tmp = dump_ + "/." + std::to_string(getpid()) + ".png";
    if (img.saveToFile(tmp)) std::rename(tmp.c_str(), path.c_str());
  }

  void writeMismatch(std::size_t index, const sf::Image& actual) {
    if (mismatchesWritten_++ >= 10) return;
    actual.saveToFile(out_ + "/" + scenario_ + "-" + std::to_string(index) + "-actual.png");
  }

  std::string scenario_, golden_, out_, dump_;
  bool update_;
  bool hasManifest_ = false;
  std::vector<Frame> expected_, actual_;
  int failures_ = 0;
  int mismatchesWritten_ = 0;
};

// A scripted input: after `wait` more frames of `window`, run `fire` and label
// the frames that follow with `label`.
struct Action {
  std::string window;
  int wait;
  std::string label;
  std::function<void()> fire;
};

struct RunawayLoop : std::runtime_error {
  RunawayLoop() : std::runtime_error("display budget exhausted") {}
};

sf::Event mouseEvent(sf::Event::EventType type, int x, int y) {
  sf::Event e;
  e.type = type;
  e.mouseButton.button = sf::Mouse::Left;
  e.mouseButton.x = x;
  e.mouseButton.y = y;
  return e;
}

sf::Event keyEvent(sf::Keyboard::Key code) {
  sf::Event e;
  e.type = sf::Event::KeyPressed;
  e.key.code = code;
  e.key.alt = e.key.control = e.key.shift = e.key.system = false;
  return e;
}

class Script {
 public:
  explicit Script(Recorder& rec, int budget = 4000) : rec_(rec), budget_(budget) {
    pin::onDisplay() = [this](sf::PinWindow& w) { onDisplay(w); };
  }
  ~Script() { pin::onDisplay() = nullptr; }

  Script& click(const std::string& window, int x, int y, const std::string& label,
                int wait = 1) {
    return then(window, wait, label, [x, y] {
      pin::mouse() = sf::Vector2i(x, y);
      pin::events().push_back(mouseEvent(sf::Event::MouseButtonPressed, x, y));
    });
  }
  Script& release(const std::string& window, int x, int y, const std::string& label,
                  int wait = 1) {
    return then(window, wait, label, [x, y] {
      pin::mouse() = sf::Vector2i(x, y);
      pin::events().push_back(mouseEvent(sf::Event::MouseButtonReleased, x, y));
    });
  }
  Script& moveMouse(const std::string& window, int x, int y, const std::string& label,
                    int wait = 1) {
    return then(window, wait, label, [x, y] { pin::mouse() = sf::Vector2i(x, y); });
  }
  Script& key(const std::string& window, sf::Keyboard::Key code, const std::string& label,
              int wait = 1) {
    return then(window, wait, label, [code] { pin::events().push_back(keyEvent(code)); });
  }
  Script& keys(const std::string& window, const std::vector<sf::Keyboard::Key>& codes,
               const std::string& label, int wait = 1) {
    return then(window, wait, label, [codes] {
      for (auto c : codes) pin::events().push_back(keyEvent(c));
    });
  }
  Script& close(const std::string& window, const std::string& label, int wait = 1) {
    return then(window, wait, label, [] {
      sf::Event e;
      e.type = sf::Event::Closed;
      pin::events().push_back(e);
    });
  }
  Script& advance(const std::string& window, sf::Time by, const std::string& label,
                  int wait = 1) {
    return then(window, wait, label, [by] { pin::now() += by; });
  }
  Script& then(const std::string& window, int wait, const std::string& label,
               std::function<void()> fire) {
    actions_.push_back(Action{window, wait, label, fire});
    return *this;
  }

  bool done() const { return actions_.empty(); }
  bool capturing = true;
  // Runs on every displayed frame, captured or not.
  std::function<void(sf::PinWindow&)> inspect;
  // Capture one display in `sampleEvery`; long runs of near-identical frames
  // (the minigame's falling box) would otherwise bloat the goldens.
  int sampleEvery = 1;

 private:
  void onDisplay(sf::PinWindow& w) {
    if (inspect) inspect(w);
    if (capturing && displays_++ % sampleEvery == 0) rec_.capture(w);
    pin::now() += kTick;
    if (!actions_.empty() && actions_.front().window == w.title() &&
        --actions_.front().wait <= 0) {
      Action a = actions_.front();
      actions_.pop_front();
      rec_.step = a.label;
      a.fire();
    }
    if (--budget_ < 0) throw RunawayLoop();
  }

  Recorder& rec_;
  int budget_;
  int displays_ = 0;
  std::deque<Action> actions_;
};

std::string readFile(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}

// Home-screen hit boxes from buttons.cpp, clicked at their centres.
const sf::Vector2i kSleep(115, 205), kShower(545, 205), kGame(975, 205), kEat(1385, 205),
    kMedicine(115, 755), kShop(545, 755), kMath(975, 755), kInfo(1385, 755),
    kTouch(775, 500);
// Every activity animation runs 6 s, 48 frames at the pin's 1/8 s tick, and
// then main draws one home frame.
const int kAfterAnimation = 49;

struct PetCase {
  std::string pet;
  sf::Vector2i species;  // intro button
  sf::Vector2i age;      // intro Adult/Baby label
  std::string shopWindow;
  std::vector<sf::Vector2i> shopClicks;
  std::string eatWindow;
  std::vector<sf::Vector2i> eatClicks;
};

const sf::Vector2i kMenuNew(960, 440), kMenuLoad(960, 600);
const sf::Vector2i kIntroAvo(330, 430), kIntroGhost(1000, 480), kIntroAdult(900, 720),
    kIntroBaby(1050, 720);

void startNewGame(Script& s, sf::Vector2i species, sf::Vector2i age) {
  s.click(kMain, kMenuLoad.x, kMenuLoad.y, "menu: load with no save")
      .click(kMain, kMenuNew.x, kMenuNew.y, "menu: new game")
      .click(kMain, species.x, species.y, "intro: pick species")
      .click(kMain, age.x, age.y, "intro: pick age");
}

int runPetJourney(Recorder& rec, const PetCase& c) {
  Script s(rec);
  startNewGame(s, c.species, c.age);
  s.click(kMain, kSleep.x, kSleep.y, "sleep", 2)
      .click(kMain, kShower.x, kShower.y, "shower", kAfterAnimation)
      .click(kMain, kMedicine.x, kMedicine.y, "medicine", kAfterAnimation)
      .click(kMain, kShop.x, kShop.y, "shop", kAfterAnimation);
  for (std::size_t i = 0; i < c.shopClicks.size(); ++i)
    s.click(c.shopWindow, c.shopClicks[i].x, c.shopClicks[i].y,
            "shop: click " + std::to_string(i + 1));
  s.close(c.shopWindow, "shop: close")
      .click(kMain, kEat.x, kEat.y, "eat");
  for (std::size_t i = 0; i < c.eatClicks.size(); ++i)
    s.click(c.eatWindow, c.eatClicks[i].x, c.eatClicks[i].y,
            "eat: click " + std::to_string(i + 1));
  s.key(c.eatWindow, sf::Keyboard::Escape, "eat: close")
      .click(kMain, kInfo.x, kInfo.y, "info", kAfterAnimation)
      .key("Information", sf::Keyboard::B, "info: close")
      .click(kMain, kTouch.x, kTouch.y, "touch pet")
      .advance(kMain, sf::seconds(30), "30 s later")
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  rec.checkText("save.txt", readFile("pet_stats.txt"));
  return rec.finish();
}

int journey(Recorder& rec, const std::string& pet) {
  // Shop and fridge clicks are item centres read off the golden frames: buy,
  // then run out of coins; eat a stocked item, then one that is not.
  if (pet == "adult_avo")
    return runPetJourney(rec, {pet, kIntroAvo, kIntroAdult, "Adult Avo Shop",
                               {{398, 203}, {748, 203}, {398, 603}, {398, 203}},
                               "Adult Avo Fridge", {{760, 340}, {1160, 340}}});
  if (pet == "baby_avo")
    return runPetJourney(rec, {pet, kIntroAvo, kIntroBaby, "Baby Avo Shop",
                               {{250, 375}, {1148, 375}, {1148, 375}},
                               "Baby Avo Fridge", {{598, 230}, {1035, 225}}});
  if (pet == "adult_ghost")
    return runPetJourney(rec, {pet, kIntroGhost, kIntroAdult, "Adult Ghost Shop",
                               {{398, 203}, {398, 603}, {398, 603}},
                               "Adult Ghost Fridge", {{760, 340}, {1160, 340}}});
  return runPetJourney(rec, {pet, kIntroGhost, kIntroBaby, "Baby Ghost Shop",
                             {{635, 320}, {885, 570}, {885, 570}},
                             "Baby Ghost Fridge", {{660, 345}, {865, 345}}});
}

int loadGame(Recorder& rec) {
  std::ofstream("pet_stats.txt") << "adult_ghost 0 0 0 0 42\n\nChicken 2\n";
  Script s(rec);
  // Apples are out of stock, so trying one leaves the pet hungry. Nothing was
  // eaten, so no eating animation plays and the home screen takes the next
  // click at once.
  s.click(kMain, kMenuLoad.x, kMenuLoad.y, "menu: load")
      .click(kMain, kEat.x, kEat.y, "eat", 2)
      .click("Adult Ghost Fridge", 1160, 340, "eat: apple")
      .key("Adult Ghost Fridge", sf::Keyboard::Escape, "eat: close")
      .click(kMain, kInfo.x, kInfo.y, "info", 2)
      .key("Information", sf::Keyboard::B, "info: close")
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  rec.checkText("save.txt", readFile("pet_stats.txt"));
  return rec.finish();
}

// The math game seeds rand() with time() in its constructor, so replaying that
// seed just before it opens yields the answers to the questions it will ask.
std::vector<int> mathAnswers() {
  std::srand(static_cast<unsigned>(pin_time(nullptr)));
  std::vector<int> answers;
  for (int i = 0; i < 5; ++i) {
    int a = std::rand() % 100, b = std::rand() % 100;
    char op = "+-*"[std::rand() % 3];
    answers.push_back(op == '+' ? a + b : op == '-' ? a - b : a * b);
  }
  return answers;
}

// Queues the keys that type `answer` and press Enter.
void typeKeys(int answer) {
  if (answer < 0) pin::events().push_back(keyEvent(sf::Keyboard::Dash));
  for (char d : std::to_string(std::abs(answer)))
    pin::events().push_back(
        keyEvent(static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + (d - '0'))));
  pin::events().push_back(keyEvent(sf::Keyboard::Enter));
}

int games(Recorder& rec) {
  Script s(rec);
  startNewGame(s, kIntroAvo, kIntroBaby);
  auto answers = std::make_shared<std::vector<int>>();
  s.then(kMain, 2, "math", [answers] {
    *answers = mathAnswers();
    pin::mouse() = kMath;
    pin::events().push_back(mouseEvent(sf::Event::MouseButtonPressed, kMath.x, kMath.y));
  });
  // Intro animation runs 3 s (24 frames), then the rules page waits for Enter.
  s.key("Math Game", sf::Keyboard::Enter, "math: start", 25);
  auto typeAnswer = [&s, answers](int q, int offset, const std::string& label) {
    s.then("Math Game", 2, label, [answers, q, offset] { typeKeys((*answers)[q] + offset); });
  };
  typeAnswer(0, 0, "math: q1 right");
  s.keys("Math Game", {sf::Keyboard::Num7, sf::Keyboard::BackSpace, sf::Keyboard::A},
         "math: q2 edit keys");
  typeAnswer(1, 0, "math: q2 right");
  typeAnswer(2, 1, "math: q3 wrong");
  typeAnswer(3, 0, "math: q4 right");
  typeAnswer(4, 1, "math: q5 wrong");
  s.key("Math Game", sf::Keyboard::Escape, "math: close results");
  s.click(kMain, kGame.x, kGame.y, "minigame", 2)
      .then("PetPal", 1, "minigame: start", [&s] {
        s.sampleEvery = 25;
        pin::events().push_back(keyEvent(sf::Keyboard::Enter));
      })
      .then("PetPal", 300, "minigame: quit", [&s] {
        s.sampleEvery = 1;
        pin::events().push_back(keyEvent(sf::Keyboard::Escape));
      })
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  rec.checkText("save.txt", readFile("pet_stats.txt"));
  return rec.finish();
}

// The save is written only when what it holds changes, and replaced whole: a
// second name for the old file still reads the old save afterwards.
int saveWrites(Recorder& rec) {
  Script s(rec);
  s.capturing = false;
  startNewGame(s, kIntroAvo, kIntroBaby);
  auto stats = [] {
    std::optional<SaveFile> save = readSave("pet_stats.txt");
    return save ? std::to_string(save->stats.health) + "/" + std::to_string(save->stats.sleep)
                : std::string("no save");
  };
  // Each check waits one frame past the display it follows, because the game
  // saves after displaying a frame.
  s.then(kMain, 3, "idle: delete the save", [&rec, stats] {
         rec.check(stats() == "25/25", "a new game writes its save, got " + stats());
         std::remove("pet_stats.txt");
       })
      .then(kMain, 5, "idle: save stays deleted", [&rec] {
        rec.check(!std::ifstream("pet_stats.txt"), "idle frames rewrote the save");
      })
      .click(kMain, kSleep.x, kSleep.y, "sleep")
      .then(kMain, kAfterAnimation + 1, "after sleep: hold the save", [&rec, stats] {
        rec.check(stats() == "25/100", "sleeping saves sleep 100, got " + stats());
        rec.check(link("pet_stats.txt", "held.txt") == 0, "could not link the save");
      })
      .click(kMain, kShower.x, kShower.y, "shower")
      .then(kMain, kAfterAnimation + 1, "after shower: check both names", [&rec, stats] {
        rec.check(stats() == "100/100", "showering saves health 100, got " + stats());
        std::optional<SaveFile> held = readSave("held.txt");
        rec.check(held && held->stats.health == 25,
                  "the old save was overwritten in place instead of replaced");
      })
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  return rec.finish();
}

// A save that fails to write is tried again on the next frame, not only once
// the pet's stats next change. A non-empty directory where the temporary file
// goes makes every write fail until the scenario removes it.
int saveRetry(Recorder& rec) {
  mkdir("pet_stats.txt.tmp", 0755);
  std::ofstream("pet_stats.txt.tmp/block") << "x";
  Script s(rec);
  s.capturing = false;
  startNewGame(s, kIntroAvo, kIntroBaby);
  s.then(kMain, 3, "idle: unblock the save", [&rec] {
     rec.check(!std::ifstream("pet_stats.txt"), "a save was written through a blocked temp file");
     std::remove("pet_stats.txt.tmp/block");
     std::remove("pet_stats.txt.tmp");
   })
      .then(kMain, 2, "idle: save written", [&rec] {
        rec.check(readSave("pet_stats.txt").has_value(), "the failed save was not tried again");
      })
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  return rec.finish();
}

// Closing the home window while the shop is open quits once the shop closes,
// and the save keeps what was bought. The pin has one event queue, so the
// shop's close and the home window's close are queued together: the shop
// takes the first and the home screen finds the second when the shop returns.
int closeDuringShop(Recorder& rec) {
  Script s(rec);
  s.capturing = false;
  startNewGame(s, kIntroAvo, kIntroAdult);
  s.click(kMain, kShop.x, kShop.y, "shop", 2)
      .click("Adult Avo Shop", 398, 203, "shop: buy steak")
      .then("Adult Avo Shop", 1, "close the shop and the home window", [] {
        sf::Event e;
        e.type = sf::Event::Closed;
        pin::events().push_back(e);
        pin::events().push_back(e);
      });
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  std::optional<SaveFile> save = readSave("pet_stats.txt");
  rec.check(save && save->stats.money == 500 && save->pantry["Steak"] == 1,
            "the save lost the steak: " + readFile("pet_stats.txt"));
  return rec.finish();
}

// On a screen too small for 1920x1080, as on a 13-inch MacBook, every window
// opens at 1470x802. The scene scales by 802/1080 on both axes to 1426x802 and
// sits centred, with black bars 22 px wide at the left and right. Clicks land
// where the scaled scene draws each button, and a click in a bar hits nothing.
int smallScreen(Recorder& rec) {
  pin::screen() = sf::Vector2u(1470, 802);
  const float kLeft = 22, kScale = 802.f / 1080;
  auto px = [=](sf::Vector2i scene) {
    return sf::Vector2i(std::lround(kLeft + scene.x * kScale), std::lround(scene.y * kScale));
  };
  const sf::Vector2i steak = px({398, 203}), menuNew = px(kMenuNew), avo = px(kIntroAvo),
                     adult = px(kIntroAdult), shop = px(kShop), info = px(kInfo),
                     math = px(kMath), game = px(kGame), fridgeSteak = px({760, 340});
  // The first pixel column of the Eat button, whose scene x range starts at
  // 1310. A click there lands only if the mapping is exact.
  const sf::Vector2i eatEdge(static_cast<int>(std::ceil(kLeft + 1310 * kScale)), px(kEat).y);
  const sf::Vector2i leftBar(10, px(kSleep).y);

  std::set<std::string> reported;
  Script s(rec);
  s.inspect = [&rec, &reported](sf::PinWindow& w) {
    sf::Image img = w.getTexture().copyToImage();
    for (unsigned x : {0u, 21u, 1448u, 1469u})
      for (unsigned y : {0u, 401u, 801u}) {
        sf::Color c = img.getPixel(x, y);
        if ((c.r == 0 && c.g == 0 && c.b == 0) || !reported.insert(w.title()).second) continue;
        rec.fail("[" + w.title() + " / " + rec.step + "] bar pixel (" + std::to_string(x) + ", " +
                 std::to_string(y) + ") is (" + std::to_string(c.r) + ", " + std::to_string(c.g) +
                 ", " + std::to_string(c.b) + "), want black");
      }
  };
  s.click(kMain, menuNew.x, menuNew.y, "menu: new game")
      .click(kMain, avo.x, avo.y, "intro: pick avo")
      .click(kMain, adult.x, adult.y, "intro: pick adult")
      .click(kMain, leftBar.x, leftBar.y, "left bar beside sleep", 2)
      .click(kMain, shop.x, shop.y, "shop")
      .click("Adult Avo Shop", steak.x, steak.y, "shop: buy steak")
      .click("Adult Avo Shop", steak.x, steak.y, "shop: buy steak")
      .close("Adult Avo Shop", "shop: close")
      .click(kMain, eatEdge.x, eatEdge.y, "eat: left edge of the button")
      .click("Adult Avo Fridge", fridgeSteak.x, fridgeSteak.y, "eat: steak")
      .key("Adult Avo Fridge", sf::Keyboard::Escape, "eat: close")
      .click(kMain, info.x, info.y, "info", kAfterAnimation)
      .key("Information", sf::Keyboard::B, "info: close")
      .click(kMain, math.x, math.y, "math")
      .key("Math Game", sf::Keyboard::Escape, "math: escape during intro", 2)
      .click(kMain, game.x, game.y, "minigame")
      .key("PetPal", sf::Keyboard::Escape, "minigame: escape", 2)
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish at step \"" + rec.step + "\": a click missed");
  std::optional<SaveFile> save = readSave("pet_stats.txt");
  rec.check(save && save->stats.sleep == 25, "the click in the left bar started sleep");
  rec.checkText("save.txt", readFile("pet_stats.txt"));
  return rec.finish();
}

// A save the game cannot read counts as no save: Load shows "No Previous Games
// Found" and a new game still starts.
int badSave(Recorder& rec) {
  std::ofstream("pet_stats.txt") << "adult_avo 1 2 three\n";
  Script s(rec);
  s.capturing = false;
  startNewGame(s, kIntroGhost, kIntroBaby);
  s.close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  std::optional<SaveFile> save = readSave("pet_stats.txt");
  rec.check(save.has_value(), "the new game did not replace the unreadable save");
  return rec.finish();
}

// Closing the window on the start menu or the pet picker quits, and leaves no
// save behind.
int closeQuits(Recorder& rec) {
  auto quitsAfter = [&rec](const std::string& where, std::function<void(Script&)> steps) {
    Script s(rec, 500);
    s.capturing = false;
    steps(s);
    int code = -1;
    try {
      code = petpal_main();
    } catch (const RunawayLoop&) {
      rec.fail("closing the " + where + " left the game running");
    }
    rec.check(code == 0, "closing the " + where + ": main returned " + std::to_string(code));
    rec.check(s.done(), "closing the " + where + ": script did not finish");
    rec.check(!std::ifstream("pet_stats.txt"), "closing the " + where + " wrote a save");
  };
  quitsAfter("start menu", [](Script& s) { s.close(kMain, "menu: close window"); });
  quitsAfter("pet picker", [](Script& s) {
    s.click(kMain, kMenuNew.x, kMenuNew.y, "menu: new game")
        .close(kMain, "picker: close window");
  });
  return rec.finish();
}

// Odd answers never crash the math game: a lone "-" is not an answer, Backspace
// removes it, "--" cancels out, and digits past what an int holds are ignored. Score 4 of 5 shows the
// lone "-" was not counted as an answer.
int mathOddAnswers(Recorder& rec) {
  Script s(rec);
  s.capturing = false;
  startNewGame(s, kIntroAvo, kIntroAdult);
  auto answers = std::make_shared<std::vector<int>>();
  s.then(kMain, 2, "math", [answers] {
    *answers = mathAnswers();
    pin::mouse() = kMath;
    pin::events().push_back(mouseEvent(sf::Event::MouseButtonPressed, kMath.x, kMath.y));
  });
  s.key("Math Game", sf::Keyboard::Enter, "math: start", 25)
      .keys("Math Game", {sf::Keyboard::Dash, sf::Keyboard::Enter}, "math: answer -", 2)
      .then("Math Game", 2, "math: q1 right after --", [answers] {
        pin::events().push_back(keyEvent(sf::Keyboard::BackSpace));
        pin::events().push_back(keyEvent(sf::Keyboard::Dash));
        pin::events().push_back(keyEvent(sf::Keyboard::Dash));
        typeKeys((*answers)[0]);
      })
      .then("Math Game", 2, "math: q2 twelve nines", [] {
        for (int i = 0; i < 12; ++i) pin::events().push_back(keyEvent(sf::Keyboard::Num9));
        pin::events().push_back(keyEvent(sf::Keyboard::Enter));
      });
  for (int q = 2; q < 5; ++q)
    s.then("Math Game", 2, "math: q" + std::to_string(q + 1) + " right",
           [answers, q] { typeKeys((*answers)[q]); });
  s.key("Math Game", sf::Keyboard::Escape, "math: close results")
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: the game stopped early");
  std::optional<SaveFile> save = readSave("pet_stats.txt");
  rec.check(save && save->stats.iq == 50 + 4 * 5,
            "math score 4 should add 20 IQ to 50, got IQ " +
                (save ? std::to_string(save->stats.iq) : std::string("(no save)")));
  return rec.finish();
}

// Escape during the math intro closes the math game and returns to the pet.
int mathEscapeIntro(Recorder& rec) {
  Script s(rec, 300);
  s.capturing = false;
  startNewGame(s, kIntroAvo, kIntroAdult);
  s.click(kMain, kMath.x, kMath.y, "math", 2)
      .key("Math Game", sf::Keyboard::Escape, "math: escape during intro", 2)
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: Escape did not leave the math intro");
  return rec.finish();
}

const std::vector<std::string> kScenarios = {
    "journey_adult_avo", "journey_baby_avo", "journey_adult_ghost",
    "journey_baby_ghost", "load_game",     "games",            "close_quits", "bad_save", "save_writes", "save_retry", "close_during_shop", "small_screen",
    "math_odd_answers", "math_escape_intro"};

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: pin <scenario> | pin --list\n";
    return 2;
  }
  std::string name = argv[1];
  if (name == "--list") {
    for (const auto& s : kScenarios) std::cout << s << "\n";
    return 0;
  }
  // Production logs to stdout on every frame; keep the pin's own output readable.
  std::cout.rdbuf(nullptr);
  Recorder rec(name);
  try {
    if (name.rfind("journey_", 0) == 0) return journey(rec, name.substr(8));
    if (name == "load_game") return loadGame(rec);
    if (name == "games") return games(rec);
    if (name == "close_quits") return closeQuits(rec);
    if (name == "bad_save") return badSave(rec);
    if (name == "save_writes") return saveWrites(rec);
    if (name == "save_retry") return saveRetry(rec);
    if (name == "close_during_shop") return closeDuringShop(rec);
    if (name == "small_screen") return smallScreen(rec);
    if (name == "math_odd_answers") return mathOddAnswers(rec);
    if (name == "math_escape_intro") return mathEscapeIntro(rec);
  } catch (const std::exception& e) {
    rec.fail(std::string("threw: ") + e.what() + " during step \"" + rec.step + "\"");
    return rec.finish();
  }
  std::cerr << "unknown scenario " << name << "\n";
  return 2;
}
