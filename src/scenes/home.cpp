#include <iostream>

#include "../anim.h"
#include "../minigames/boxes.h"
#include "../minigames/math.h"
#include "../save.h"
#include "scenes.h"

namespace {

enum class Button { Sleep, Shower, Game, Eat, Medicine, Shop, Info, Math, Touch, None };

struct ButtonSpot {
  Button button;
  const char* icon;  // null for the pet itself
  int x, y, w, h;
};

const ButtonSpot kButtons[] = {
    {Button::Sleep, "assets/ui/moon.png", 40, 130, 150, 150},
    {Button::Shower, "assets/ui/shower.png", 470, 130, 150, 150},
    {Button::Game, "assets/ui/console.png", 900, 130, 150, 150},
    {Button::Eat, "assets/ui/cutlery.png", 1310, 130, 150, 150},
    {Button::Medicine, "assets/ui/first-aid-kit.png", 40, 680, 150, 150},
    {Button::Shop, "assets/ui/grocery-cart.png", 470, 680, 150, 150},
    {Button::Info, "assets/ui/info.png", 1310, 680, 150, 150},
    {Button::Math, "assets/ui/math.png", 900, 680, 150, 150},
    {Button::Touch, nullptr, 650, 400, 250, 200},
};

Button hitTest(sf::Vector2i p) {
  for (const ButtonSpot& b : kButtons)
    if (p.x >= b.x && p.x <= b.x + b.w && p.y >= b.y && p.y <= b.y + b.h) return b.button;
  return Button::None;
}

// Everything drawn on the home screen that is not a number.
struct HomeArt {
  std::vector<sf::Texture> textures;
  std::vector<sf::Sprite> icons;
  sf::RectangleShape line;
  sf::Texture idleTexture, coinTexture;
  sf::Sprite idle;

  bool load(const PetSpec& pet) {
    bool ok = true;
    line.setSize(sf::Vector2f(1920, 5));
    line.setFillColor(sf::Color::Black);
    line.setPosition(0, 120);
    textures.resize(std::size(kButtons));
    for (std::size_t i = 0; i < std::size(kButtons); ++i) {
      if (!kButtons[i].icon) continue;
      if (!textures[i].loadFromFile(kButtons[i].icon)) ok = false;
      icons.emplace_back(textures[i]);
      icons.back().setPosition(kButtons[i].x, kButtons[i].y);
    }
    if (!idleTexture.loadFromFile(pet.idleSprite)) ok = false;
    idle.setTexture(idleTexture);
    idle.setPosition(600, 350);
    idle.setScale(8.f, 8.f);
    if (!coinTexture.loadFromFile("assets/ui/coin.png")) ok = false;
    return ok;
  }

  void drawChrome(sf::RenderTarget& target) const {
    target.clear(sf::Color(230, 230, 220));
    target.draw(line);
    for (const sf::Sprite& icon : icons) target.draw(icon);
  }
};

void drawBar(sf::RenderTarget& target, const sf::Font& font, const char* label, float width,
             float barX, float labelX) {
  sf::RectangleShape bar(sf::Vector2f(width, 30));
  bar.setFillColor(sf::Color::Black);
  bar.setPosition(barX, 50);
  target.draw(bar);
  target.draw(makeText(font, 44, sf::Color::Black, labelX, 2, label));
}

void drawStats(sf::RenderTarget& target, const sf::Font& font, const Stats& stats,
               const sf::Texture& coin) {
  // The first three bars truncate to whole pixels; the IQ bar does not.
  drawBar(target, font, "Health Bar: ", 250 * stats.health / 100, 20, 50);
  drawBar(target, font, "Sleep Bar: ", 250 * stats.sleep / 100, 350, 350);
  drawBar(target, font, "Hunger Bar: ", 250 * stats.hunger / 100, 970, 970);
  drawBar(target, font, "IQ Bar: ", 250 * stats.iq / 100.0f, 650, 650);
  target.draw(makeText(font, 44, sf::Color::Black, 1320, 20, std::to_string(stats.money)));
  sf::Sprite coinSign(coin);
  coinSign.setPosition(1240, 20);
  coinSign.setScale(0.1f, 0.1f);
  target.draw(coinSign);
}

void drawWarnings(sf::RenderTarget& target, const sf::Font& font, const Stats& stats) {
  const std::pair<int, const char*> checks[] = {{stats.health, "WARNING!! HEALTH IS 0"},
                                                {stats.sleep, "WARNING!! SLEEP IS 0"},
                                                {stats.hunger, "WARNING!! HUNGER IS 0"},
                                                {stats.iq, "WARNING!! IQ IS 0"}};
  float y = 400;
  for (const auto& [level, warning] : checks) {
    if (level > 0) continue;
    target.draw(makeText(font, 44, sf::Color::Red, 20, y, warning));
    y += 60;
  }
}

// Plays the activity's sheet over the home screen for 6 seconds.
void runActivity(sf::RenderWindow& window, sf::Font& font, const HomeArt& art,
                 const PetSpec& pet, Activity activity, const Quote& quote,
                 const Stats& stats) {
  SheetPlayer anim(pet.sheets[static_cast<int>(activity)], activity);
  sf::Clock frameTimer;
  sf::Clock total;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
    }
    anim.update(frameTimer.restart().asSeconds());
    art.drawChrome(window);
    anim.draw(window);
    drawQuote(window, font, quote);
    drawStats(window, font, stats, art.coinTexture);
    window.display();
    if (total.getElapsedTime().asSeconds() >= 6) break;
  }
}

}  // namespace

int runHome(sf::RenderWindow& window, sf::Font& font, sf::Music& music, const PetSpec& pet,
            Stats& stats, Pantry& pantry, Quote& quote) {
  HomeArt art;
  if (!art.load(pet)) std::cerr << "error loading sprites" << std::endl;
  sf::Music petSound;
  if (!petSound.openFromFile(soundPath(pet.species)))
    std::cerr << "Failed to load pet sound!" << std::endl;
  petSound.setVolume(100);

  std::string written;  // the save file's text as last written
  sf::Clock decayTimer;

  while (window.isOpen()) {
    if (decayTimer.getElapsedTime().asSeconds() >= 30) {
      stats.decay();
      decayTimer.restart();
    }
    updateQuote(quote, pet.species);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        return 0;
      }
      if (event.type != sf::Event::MouseButtonPressed) continue;
      switch (hitTest(sf::Mouse::getPosition(window))) {
        case Button::Sleep:
          stats.sleep = 100;
          runActivity(window, font, art, pet, Activity::Sleep, quote, stats);
          break;
        case Button::Shower:
          stats.health = 100;
          runActivity(window, font, art, pet, Activity::Shower, quote, stats);
          break;
        case Button::Medicine:
          stats.health = 100;
          runActivity(window, font, art, pet, Activity::Medicine, quote, stats);
          break;
        case Button::Eat:
          runFridge(pet, font, stats, pantry);
          runActivity(window, font, art, pet, Activity::Eat, quote, stats);
          break;
        case Button::Game: {
          music.pause();
          mini_game game;
          while (game.window_open()) {
            game.update();
            game.render();
          }
          stats.money += game.money_earned_end();
          music.play();
          break;
        }
        case Button::Math: {
          music.pause();
          math_game game;
          stats.addIQ(game.getScore());
          music.play();
          break;
        }
        case Button::Shop:
          stats.money = runShop(pet, font, stats.money, pantry);
          break;
        case Button::Info:
          runInfo(font);
          break;
        case Button::Touch:
          if (petSound.getStatus() != sf::SoundSource::Playing) petSound.play();
          break;
        case Button::None:
          break;
      }
    }

    art.drawChrome(window);
    drawStats(window, font, stats, art.coinTexture);
    drawQuote(window, font, quote);
    window.draw(art.idle);
    drawWarnings(window, font, stats);
    window.display();
    std::string save = formatSave(pet, stats, pantry);
    if (save != written) {
      writeSave("pet_stats.txt", save);
      written = save;
    }
  }
  return 0;
}
