// Characterization pin. Each scenario runs real production code (main() and
// the classes it uses) against the fakes in shim.h and compares every frame
// the game displays with tests/golden. Usage: pin <scenario> | pin --list.
// Env: PIN_GOLDEN (golden dir), PIN_OUT (where mismatches go), PIN_UPDATE=1
// (rewrite the manifests instead of comparing), PIN_DUMP (absolute dir; also
// save every displayed frame there as <hash prefix>.png).

// Test-only access to private members (mini_game scoring), so the pin needs
// no change to production code.
#include <CommonCrypto/CommonDigest.h>
#include <unistd.h>

#define private public
#define main petpal_main
#include "../../src/main.cpp"
#include "../../src/minigames/boxes.h"
#undef main
#undef private

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
  // Capture one display in `sampleEvery`; long runs of near-identical frames
  // (the minigame's falling box) would otherwise bloat the goldens.
  int sampleEvery = 1;

 private:
  void onDisplay(sf::PinWindow& w) {
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

void startNewGame(Script& s, const PetCase& c) {
  s.click(kMain, kMenuLoad.x, kMenuLoad.y, "menu: load with no save")
      .click(kMain, kMenuNew.x, kMenuNew.y, "menu: new game")
      .click(kMain, c.species.x, c.species.y, "intro: pick species")
      .click(kMain, c.age.x, c.age.y, "intro: pick age");
}

int runPetJourney(Recorder& rec, const PetCase& c) {
  Script s(rec);
  startNewGame(s, c);
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
    return runPetJourney(rec, {pet, kIntroAvo, kIntroAdult, "Adult Avo Shopping",
                               {{398, 203}, {748, 203}, {398, 603}, {398, 203}},
                               "Eating Adult Avo", {{760, 340}, {1160, 340}}});
  if (pet == "baby_avo")
    return runPetJourney(rec, {pet, kIntroAvo, kIntroBaby, "Baby Avo Shop",
                               {{250, 375}, {1148, 375}, {1148, 375}},
                               "Eating Baby Avo", {{598, 230}, {1035, 225}}});
  if (pet == "adult_ghost")
    return runPetJourney(rec, {pet, kIntroGhost, kIntroAdult, "Shopping",
                               {{398, 203}, {398, 603}, {398, 603}},
                               "Eating Ghost Window", {{760, 340}, {1160, 340}}});
  return runPetJourney(rec, {pet, kIntroGhost, kIntroBaby, "Baby Ghost Shopping",
                             {{635, 320}, {885, 570}, {885, 570}},
                             "Eating Baby Ghost", {{660, 345}, {865, 345}}});
}

int loadGame(Recorder& rec) {
  std::ofstream("pet_stats.txt") << "adult_ghost 0 0 0 0 42\n\nSteak 2\n";
  Script s(rec);
  s.click(kMain, kMenuLoad.x, kMenuLoad.y, "menu: load")
      .click(kMain, kEat.x, kEat.y, "eat", 2)
      .key("Eating Ghost Window", sf::Keyboard::Escape, "eat: close")
      .close(kMain, "quit", kAfterAnimation);
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
  startNewGame(s, {"baby_avo", kIntroAvo, kIntroBaby});
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

// Known bug, pinned as-is: closing the window on the start menu is treated as
// "new game" and the intro loop then spins forever on the closed window.
int menuCloseSpins(Recorder& rec) {
  Script s(rec, 500);
  s.capturing = false;
  s.close(kMain, "menu: close window");
  bool spun = false;
  try {
    petpal_main();
  } catch (const RunawayLoop&) {
    spun = true;
  }
  rec.check(spun, "closing the start menu no longer spins (bug fixed? update this pin)");
  return rec.finish();
}

// Odd answers never crash the math game: a lone "-" is not an answer, Backspace
// removes it, "--" cancels out, and digits past what an int holds are ignored. Score 4 of 5 shows the
// lone "-" was not counted as an answer.
int mathOddAnswers(Recorder& rec) {
  Script s(rec);
  s.capturing = false;
  startNewGame(s, {"adult_avo", kIntroAvo, kIntroAdult});
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
  startNewGame(s, {"adult_avo", kIntroAvo, kIntroAdult});
  s.click(kMain, kMath.x, kMath.y, "math", 2)
      .key("Math Game", sf::Keyboard::Escape, "math: escape during intro", 2)
      .close(kMain, "quit", 2);
  int code = petpal_main();
  rec.check(code == 0, "main returned " + std::to_string(code));
  rec.check(s.done(), "script did not finish: Escape did not leave the math intro");
  return rec.finish();
}

int logic(Recorder& rec) {
  auto show = [](const Stats& s) {
    return std::to_string(s.health) + "/" + std::to_string(s.sleep) + "/" +
           std::to_string(s.hunger) + "/" + std::to_string(s.iq) + "/" + std::to_string(s.money);
  };
  auto expectEq = [&rec](const std::string& got, const std::string& want,
                         const std::string& what) {
    rec.check(got == want, what + ": got \"" + got + "\", want \"" + want + "\"");
  };

  {
    Stats s;
    expectEq(show(s), "25/25/25/50/1000", "new pet stats");
    s.decay();
    expectEq(show(s), "15/15/15/45/1000", "decay: -10 each, IQ -5");
    s.decay();
    s.decay();
    expectEq(show(s), "0/0/0/35/1000", "decay floors at 0");
    s.addIQ(5);
    expectEq(std::to_string(s.iq), "60", "addIQ adds 5 per point");
    s.addIQ(6);
    s.addIQ(-1);
    expectEq(std::to_string(s.iq), "60", "addIQ ignores scores outside 0..5");
    s.addIQ(5);
    s.addIQ(5);
    s.addIQ(5);
    expectEq(std::to_string(s.iq), "100", "IQ caps at 100");
  }
  {
    std::ofstream("pin_read.txt") << "baby_ghost 1 2 3 4 5\n\nSteak 2\n";
    std::optional<SaveFile> save = readSave("pin_read.txt");
    rec.check(save.has_value(), "readSave reads a well-formed file");
    if (save) {
      expectEq(save->pet + " " + show(save->stats), "baby_ghost 1/2/3/4/5",
               "readSave reads the pet and the five numbers");
      expectEq(std::to_string(save->pantry.size()) + ":" + std::to_string(save->pantry["Steak"]),
               "1:2", "readSave reads the pantry lines");
    }
    rec.check(!readSave("no_such_file.txt"), "readSave on a missing file is empty");
    std::remove("pin_read.txt");
  }
  {
    const PetSpec* a = findPet("baby_avo");
    const PetSpec* g = findPet("adult_ghost");
    rec.check(a && a->species == Species::Avo && a->stage == Stage::Baby, "findPet(baby_avo)");
    rec.check(g && g->species == Species::Ghost && g->stage == Stage::Adult,
              "findPet(adult_ghost)");
    rec.check(findPet("cat") == nullptr, "unknown pet names have no spec");
  }
  {
    // Quotes are picked by srand(time(0)); rand(), so the pick is a function of
    // the wall-clock second.
    std::string picks;
    for (int s = 0; s < 4; ++s) {
      pin::now() = sf::seconds(static_cast<float>(s));
      picks += pickQuote(Species::Avo) + "|" + pickQuote(Species::Ghost) + "\n";
    }
    rec.checkText("quotes.txt", picks);

    pin::now() = sf::Time::Zero;
    Quote quote;
    quote.text = "first";
    pin::now() = sf::seconds(9.875f);
    updateQuote(quote, Species::Avo);
    rec.check(quote.text == "first" && !quote.visible, "quote kept before 10 s");
    pin::now() = sf::seconds(10);
    updateQuote(quote, Species::Avo);
    rec.check(quote.text == pickQuote(Species::Avo) && quote.visible, "new visible quote at 10 s");
    pin::now() = sf::seconds(19.875f);
    updateQuote(quote, Species::Avo);
    rec.check(quote.visible, "known bug pinned: quote stays visible (never hides)");
  }
  {
    mini_game g;
    rec.check(g.window_open() && g.money_earned_end() == 0 && g.points == 0,
              "new minigame: window open, nothing earned");
    rec.check(g.in_intro_game, "minigame starts on the intro screen");
    std::string table;
    for (float height : {50.f, 59.f, 60.f, 79.f, 80.f, 99.f, 100.f, 119.f, 120.f, 149.f,
                         150.f, 199.f, 200.f}) {
      unsigned before = g.points;
      float speed = g.box_falling_speed;
      sf::RectangleShape box(sf::Vector2f(50.f, height));
      box.setPosition(g.slider_knob.getPosition());
      g.boxes.assign(1, box);
      g.update_points();
      table += std::to_string(static_cast<int>(height)) + ":" +
               std::to_string(g.points - before) + (g.boxes.empty() ? "" : "!kept") +
               (g.box_falling_speed == speed + 0.5f ? "" : "!speed") + " ";
    }
    expectEq(table,
             "50:6 59:6 60:5 79:5 80:4 99:4 100:3 119:3 120:2 149:2 150:1 199:1 200:0 ",
             "points by caught box height; each catch removes the box and speeds up 0.5");
    g.update_game_over_text();
    expectEq(std::to_string(g.money_earned_end()), std::to_string(g.points * 5),
             "coins earned are 5 per point");
    g.boxes.clear();
    g.box_spawn_timer = g.box_spawn_timer_max;
    g.update_boxes();
    rec.check(g.boxes.size() == 1, "a box spawns once the spawn timer is full");
    std::srand(7);
    std::vector<std::string> songs = {"a", "b", "c", "d"};
    std::string pick = g.get_random_song(songs);
    std::srand(7);
    expectEq(pick, songs[std::rand() % songs.size()], "song pick is rand() % count");
  }
  return rec.finish();
}

const std::vector<std::string> kScenarios = {
    "logic",          "journey_adult_avo", "journey_baby_avo", "journey_adult_ghost",
    "journey_baby_ghost", "load_game",     "games",            "menu_close_spins",
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
    if (name == "logic") return logic(rec);
    if (name.rfind("journey_", 0) == 0) return journey(rec, name.substr(8));
    if (name == "load_game") return loadGame(rec);
    if (name == "games") return games(rec);
    if (name == "menu_close_spins") return menuCloseSpins(rec);
    if (name == "math_odd_answers") return mathOddAnswers(rec);
    if (name == "math_escape_intro") return mathEscapeIntro(rec);
  } catch (const std::exception& e) {
    rec.fail(std::string("threw: ") + e.what() + " during step \"" + rec.step + "\"");
    return rec.finish();
  }
  std::cerr << "unknown scenario " << name << "\n";
  return 2;
}
