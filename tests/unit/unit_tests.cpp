// Logic checks that need no window. Built with tests/pin/shim.h force-included,
// so sf::Clock reads the fake clock (pin::now()) and nothing opens on screen.
#include <fstream>
#include <algorithm>
#include <iostream>
#include <set>
#include <string>

#include "../../src/anim.h"
#include "../../src/catalog.h"
#include "../../src/minigames/boxes.h"
#include "../../src/minigames/math.h"
#include "../../src/quote.h"
#include "../../src/save.h"
#include "../../src/stats.h"

namespace {

std::set<std::string> failures;

void check(bool ok, const std::string& name, const std::string& detail = "") {
  if (ok) {
    std::cout << name << " Passed" << std::endl;
  } else {
    failures.insert(name);
    std::cout << name << " Failed" << (detail.empty() ? "" : ": " + detail) << std::endl;
  }
}

void miniGameTest() {
  mini_game game;
  check(game.window_open() && game.money_earned_end() == 0, "[Mini Game Test]");
}

void statsTests() {
  Stats s;
  check(s.health == 25 && s.sleep == 25 && s.hunger == 25 && s.iq == 50 && s.money == 1000,
        "[Test Initial Values]");
  s.decay();
  check(s.health == 15 && s.sleep == 15 && s.hunger == 15 && s.iq == 45, "[Test Decay]");
  s.decay();
  s.decay();
  check(s.health == 0 && s.sleep == 0 && s.hunger == 0 && s.iq == 35, "[Test Decay Floor]");
  s.addIQ(5);
  s.addIQ(6);
  check(s.iq == 60, "[Test Add IQ]", "got " + std::to_string(s.iq));
}

void sheetPlayerAdvancesFrames() {
  const PetSpec& pet = findPet(Species::Avo, Stage::Adult);
  SheetPlayer shower(pet.sheets[static_cast<int>(Activity::Shower)], Activity::Shower);
  shower.update(1.4f);
  shower.update(1.4f);
  check(shower.frame() == 2 && shower.rect().left == 64, "[SheetPlayer Frame Test]",
        "frame " + std::to_string(shower.frame()) + " left " +
            std::to_string(shower.rect().left));
}

void sheetPlayerEatingHalvesLaterFrames() {
  const PetSpec& pet = findPet(Species::Ghost, Stage::Baby);
  SheetPlayer eat(pet.sheets[static_cast<int>(Activity::Eat)], Activity::Eat);
  eat.update(1.4f);  // frame 1 after a full duration
  eat.update(0.7f);  // frame 2 after half
  eat.update(0.7f);  // back to frame 0
  check(eat.frame() == 0 && eat.rect().top == 0, "[SheetPlayer Eating Test]",
        "frame " + std::to_string(eat.frame()));
}

// Every frame of every pet's sheets samples cells that lie inside the image.
void sheetsFitTheirImages() {
  for (Species species : {Species::Avo, Species::Ghost}) {
    for (Stage stage : {Stage::Adult, Stage::Baby}) {
      const PetSpec& pet = findPet(species, stage);
      for (const Sheet& sheet : pet.sheets) {
        sf::Image image;
        image.loadFromFile(sheet.path);
        int rows = (sheet.frames + sheet.cols - 1) / sheet.cols;
        bool fits = sheet.cols * sheet.cellW <= static_cast<int>(image.getSize().x) &&
                    rows * sheet.cellH <= static_cast<int>(image.getSize().y);
        check(fits, std::string("[Sheet Fits Image - ") + sheet.path + "]",
              std::to_string(sheet.cols) + "x" + std::to_string(rows) + " cells of " +
                  std::to_string(sheet.cellW) + "x" + std::to_string(sheet.cellH) + " on a " +
                  std::to_string(image.getSize().x) + "x" + std::to_string(image.getSize().y) +
                  " image");
      }
    }
  }
}

void quoteTests() {
  pin::now() = sf::Time::Zero;
  Quote quote;
  pin::now() += sf::seconds(10.1f);
  updateQuote(quote, Species::Avo);
  check(quote.visible && !quote.text.empty(), "[Avo Test - Update Quote]");
  updateQuote(quote, Species::Avo);
  pin::now() += sf::seconds(10.f);
  updateQuote(quote, Species::Avo);
  check(!quote.visible, "[Avo Test - Quote Visibility]",
        "expected the quote to hide after its display duration");
}

std::string typed(const std::string& keys) {
  AnswerInput input;
  for (char k : keys) {
    if (k == '-') input.toggleSign();
    else if (k == '<') input.erase();
    else input.addDigit(k - '0');
  }
  std::optional<int> v = input.value();
  return input.text() + "=" + (v ? std::to_string(*v) : "none");
}

void answerInputTests() {
  check(typed("") == "=none", "[Math Input - Nothing Typed]", typed(""));
  check(typed("-") == "-=none", "[Math Input - Lone Minus Is No Answer]", typed("-"));
  check(typed("-42") == "-42=-42", "[Math Input - Negative]", typed("-42"));
  check(typed("42-") == "-42=-42", "[Math Input - Minus After Digits]", typed("42-"));
  check(typed("--5") == "5=5", "[Math Input - Double Minus Cancels]", typed("--5"));
  check(typed("-5<<") == "=none", "[Math Input - Backspace Removes Sign Last]", typed("-5<<"));
  check(typed("999999999999") == "999999999=999999999", "[Math Input - Digits Capped To Int]",
        typed("999999999999"));
}

// "ok", "none", or "threw: <what>" for readSave on a file holding `text`.
std::string readSaveOf(const std::string& text) {
  const char* path = "build/unit_save.txt";
  std::ofstream(path) << text;
  try {
    return readSave(path) ? "ok" : "none";
  } catch (const std::exception& e) {
    return std::string("threw: ") + e.what();
  }
}

void readSaveTests() {
  const std::string good = "adult_ghost 1 2 3 4 5\n\nChicken 2\nBaby Milk 1\n";
  check(readSaveOf(good) == "ok", "[Save - Well Formed]", readSaveOf(good));
  const std::pair<const char*, const char*> malformed[] = {
      {"", "[Save - Empty File Is No Save]"},
      {"adult_ghost 1 2\n", "[Save - Short Stats Line Is No Save]"},
      {"cat 1 2 3 4 5\n", "[Save - Unknown Pet Is No Save]"},
      {"adult_ghost 1 2 3 4 5\n\nChicken lots\n", "[Save - Bad Count Is No Save]"},
      {"adult_ghost 1 2 3 4 5\n\nChicken 99999999999\n", "[Save - Huge Count Is No Save]"},
      {"adult_ghost 1 2 3 4 5\n\nChicken\n", "[Save - Missing Count Is No Save]"},
      {"adult_ghost 1 2 3 4 5\n\nChicken -1\n", "[Save - Negative Count Is No Save]"},
  };
  for (const auto& [text, name] : malformed) check(readSaveOf(text) == "none", name, readSaveOf(text));
}

// The drawn pixels inside the bounding box of everything not background, as
// "<w>x<h>:<rgba bytes>", so two drawings compare regardless of position.
std::string ink(const sf::RenderTexture& target, sf::Color background) {
  sf::Image img = target.getTexture().copyToImage();
  unsigned left = img.getSize().x, top = img.getSize().y, right = 0, bottom = 0;
  for (unsigned y = 0; y < img.getSize().y; ++y)
    for (unsigned x = 0; x < img.getSize().x; ++x)
      if (img.getPixel(x, y) != background) {
        left = std::min(left, x), right = std::max(right, x + 1);
        top = std::min(top, y), bottom = std::max(bottom, y + 1);
      }
  if (right == 0) return "nothing";
  std::string out = std::to_string(right - left) + "x" + std::to_string(bottom - top) + ":";
  for (unsigned y = top; y < bottom; ++y)
    for (unsigned x = left; x < right; ++x) {
      sf::Color c = img.getPixel(x, y);
      out += {static_cast<char>(c.r), static_cast<char>(c.g), static_cast<char>(c.b),
              static_cast<char>(c.a)};
    }
  return out;
}

void quoteDrawsItsCharacters() {
  sf::Font font;
  font.loadFromFile("assets/fonts/Regular.ttf");
  const sf::Color background(230, 230, 220);
  sf::RenderTexture drawn, wanted;
  drawn.create(1920, 400);
  wanted.create(1920, 400);

  Quote quote;
  quote.text = "I\u2019m here\u2014boo";  // UTF-8, as the quote list stores it
  quote.visible = true;
  drawn.clear(background);
  drawQuote(drawn, font, quote);
  drawn.display();

  sf::Text text(sf::String(L"I\u2019m here\u2014boo"), font, 39);
  text.setFillColor(sf::Color::Black);
  text.setPosition(100, 100);
  wanted.clear(background);
  wanted.draw(text);
  wanted.display();

  std::string got = ink(drawn, background), want = ink(wanted, background);
  check(got == want, "[Quote Draws Curly Apostrophe And Dash]",
        "drew " + got.substr(0, got.find(':')) + ", wanted " + want.substr(0, want.find(':')));
}

}  // namespace

int main() {
  std::cout << "Running Unit Tests..." << std::endl << std::endl;
  miniGameTest();
  statsTests();
  sheetPlayerAdvancesFrames();
  sheetPlayerEatingHalvesLaterFrames();
  sheetsFitTheirImages();
  quoteTests();
  answerInputTests();
  quoteDrawsItsCharacters();
  readSaveTests();

  for (const std::string& name : failures) std::cout << "FAIL " << name << std::endl;
  return failures.empty() ? 0 : 1;
}
