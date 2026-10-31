// Logic checks that need no window. Built with tests/pin/shim.h force-included,
// so sf::Clock reads the fake clock (pin::now()) and nothing opens on screen.
#include <iostream>
#include <set>
#include <string>

#include "../../src/anim.h"
#include "../../src/catalog.h"
#include "../../src/minigames/boxes.h"
#include "../../src/quote.h"
#include "../../src/stats.h"

namespace {

// Known bugs this suite already catches. A listed test must keep failing until
// its fix lands; the fix then removes it from this list.
const std::set<std::string> kExpectedFailures = {
    "[Avo Test - Quote Visibility]",  // quote never hides (src/quote.cpp)
};

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

}  // namespace

int main() {
  std::cout << "Running Unit Tests..." << std::endl << std::endl;
  miniGameTest();
  statsTests();
  sheetPlayerAdvancesFrames();
  sheetPlayerEatingHalvesLaterFrames();
  quoteTests();

  int unexpected = 0;
  for (const std::string& name : failures) {
    if (kExpectedFailures.count(name)) {
      std::cout << "XFAIL " << name << std::endl;
    } else {
      std::cout << "FAIL " << name << std::endl;
      unexpected++;
    }
  }
  for (const std::string& name : kExpectedFailures) {
    if (!failures.count(name)) {
      std::cout << "XPASS " << name << ": remove it from kExpectedFailures" << std::endl;
      unexpected++;
    }
  }
  return unexpected == 0 ? 0 : 1;
}
