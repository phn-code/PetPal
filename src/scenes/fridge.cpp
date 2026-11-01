#include <iostream>

#include "scenes.h"

namespace {

int inStock(const Pantry& pantry, const std::string& food) {
  auto it = pantry.find(food);
  return it == pantry.end() ? 0 : it->second;
}

}  // namespace

void runFridge(const PetSpec& pet, sf::Font& font, Stats& stats, Pantry& pantry) {
  const FridgeLayout& layout = pet.fridge;
  sf::RenderWindow window(sf::VideoMode(1920, 1080), layout.title);

  sf::Texture fridgeTexture;
  if (!fridgeTexture.loadFromFile("assets/ui/fridge.png"))
    std::cerr << "Error: Fridge image not found!" << std::endl;
  sf::Sprite fridge(fridgeTexture);
  fridge.setScale(1920.f / fridgeTexture.getSize().x, 1080.f / fridgeTexture.getSize().y);

  sf::Text message = makeText(font, 36, sf::Color::Red, 800, 100);

  FoodIcons icons = loadFoodIcons(pet.foods, layout.grid);
  std::vector<sf::Text> quantities;
  for (std::size_t i = 0; i < pet.foods.size(); ++i) {
    const sf::Sprite& sprite = icons.sprites[i];
    quantities.push_back(makeText(font, 28, sf::Color::Black, sprite.getPosition().x,
                                  sprite.getPosition().y + sprite.getGlobalBounds().height +
                                      layout.labelGap,
                                  "Quantity: " + std::to_string(inStock(pantry, pet.foods[i].name))));
  }

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        window.close();
      if (event.type != sf::Event::MouseButtonPressed) continue;
      sf::Vector2f click = toScene(window, event.mouseButton.x, event.mouseButton.y);
      message.setString("");
      for (std::size_t i = 0; i < pet.foods.size(); ++i) {
        if (!icons.sprites[i].getGlobalBounds().contains(click)) continue;
        const std::string food = pet.foods[i].name;
        int left = inStock(pantry, food);
        if (left > 0) {
          if (--left == 0)
            pantry.erase(food);
          else
            pantry[food] = left;
          stats.hunger = 100;
          quantities[i].setString("Quantity: " + std::to_string(left));
        } else {
          message.setString("NEED TO BUY MORE");
        }
      }
    }

    clearScene(window, sf::Color(245, 245, 220));
    window.draw(fridge);
    window.draw(message);
    for (std::size_t i = 0; i < pet.foods.size(); ++i) {
      window.draw(icons.sprites[i]);
      window.draw(quantities[i]);
    }
    window.display();
  }
}
