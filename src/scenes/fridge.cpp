#include <iostream>

#include "../save.h"
#include "scenes.h"

namespace {

// The four original fridge windows disagreed on two rules. Both are bugs,
// kept as pinned; phase 3 fixes them here.
struct FridgeRules {
  bool phantomStock;          // eat from a fixed 5 of each food, ignoring what was bought
  bool feedBeforeStockCheck;  // the pet is fed even when the food is out of stock
};

FridgeRules rulesFor(const PetSpec& pet) {
  return {pet.species == Species::Avo && pet.stage == Stage::Adult,
          pet.stage == Stage::Adult};
}

}  // namespace

void runFridge(const PetSpec& pet, sf::Font& font, Stats& stats) {
  const FridgeLayout& layout = pet.fridge;
  const FridgeRules rules = rulesFor(pet);
  sf::RenderWindow window(sf::VideoMode(1920, 1080), layout.title);

  sf::Texture fridgeTexture;
  if (!fridgeTexture.loadFromFile("assets/ui/fridge.png"))
    std::cerr << "Error: Fridge image not found!" << std::endl;
  sf::Sprite fridge(fridgeTexture);
  fridge.setScale(1920.f / fridgeTexture.getSize().x, 1080.f / fridgeTexture.getSize().y);

  sf::Text message = makeText(font, 36, sf::Color::Red, 800, 100);

  std::map<std::string, int> pantry;
  if (std::optional<SaveFile> save = readSave("pet_stats.txt")) pantry = save->pantry;

  FoodIcons icons = loadFoodIcons(pet.foods, layout.grid);
  std::vector<int> stock, phantom(pet.foods.size(), 5);
  std::vector<sf::Text> quantities;
  for (std::size_t i = 0; i < pet.foods.size(); ++i) {
    const sf::Sprite& sprite = icons.sprites[i];
    stock.push_back(pantry.count(pet.foods[i].name) ? pantry[pet.foods[i].name] : 0);
    quantities.push_back(makeText(font, 28, sf::Color::Black, sprite.getPosition().x,
                                  sprite.getPosition().y + sprite.getGlobalBounds().height +
                                      layout.labelGap,
                                  "Quantity: " + std::to_string(stock[i])));
  }

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        window.close();
      if (event.type != sf::Event::MouseButtonPressed) continue;
      sf::Vector2i click = sf::Mouse::getPosition(window);
      message.setString("");
      for (std::size_t i = 0; i < pet.foods.size(); ++i) {
        if (!icons.sprites[i].getGlobalBounds().contains(static_cast<float>(click.x),
                                                         static_cast<float>(click.y)))
          continue;
        if (rules.feedBeforeStockCheck) stats.hunger = 100;
        int& left = rules.phantomStock ? phantom[i] : stock[i];
        if (left > 0) {
          if (!rules.feedBeforeStockCheck) stats.hunger = 100;
          left--;
          quantities[i].setString("Quantity: " + std::to_string(left));
        } else {
          message.setString("NEED TO BUY MORE");
        }
      }
    }

    window.clear(sf::Color(245, 245, 220));
    window.draw(fridge);
    window.draw(message);
    for (std::size_t i = 0; i < pet.foods.size(); ++i) {
      window.draw(icons.sprites[i]);
      window.draw(quantities[i]);
    }
    window.display();
  }
}
