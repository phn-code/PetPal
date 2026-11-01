#include <iostream>

#include "scenes.h"

FoodIcons loadFoodIcons(const std::vector<Food>& foods, const Grid& grid) {
  FoodIcons icons;
  icons.textures.resize(foods.size());
  for (std::size_t i = 0; i < foods.size(); ++i) {
    sf::Texture& texture = icons.textures[i];
    if (!texture.loadFromFile(foods[i].icon))
      std::cerr << "Error: " << foods[i].name << " image not found at " << foods[i].icon
                << std::endl;
    sf::Sprite sprite(texture);
    sprite.setScale(150.f / texture.getSize().x, 150.f / texture.getSize().y);
    int cell = static_cast<int>(i);
    sprite.setPosition(grid.x + (cell % grid.cols) * grid.dx,
                       grid.y + (cell / grid.cols) * grid.dy);
    icons.sprites.push_back(sprite);
  }
  return icons;
}

int runShop(const PetSpec& pet, sf::Font& font, int coins, Pantry& pantry) {
  const ShopLayout& layout = pet.shop;
  sf::RenderWindow window(sf::VideoMode(1920, 1080), layout.title);

  int trolleyCount = 0;
  sf::Text coinsText = makeText(font, 36, sf::Color::Black, 20, 20, "Coins: " + std::to_string(coins));
  sf::Text trolleyText = makeText(font, 36, sf::Color::Black, 1700, 20, "Trolley: 0");
  sf::Text noCoinsText = makeText(font, 36, sf::Color::Red, 800, 100);

  sf::Texture trolleyTexture;
  if (!trolleyTexture.loadFromFile("assets/ui/trolley.png"))
    std::cerr << "Error: Trolley image not found at assets/ui/trolley.png" << std::endl;
  sf::Sprite trolleySprite(trolleyTexture);
  trolleySprite.setPosition(1700, 70);
  trolleySprite.setScale(0.5f, 0.5f);

  FoodIcons icons = loadFoodIcons(pet.foods, layout.grid);
  std::vector<sf::Text> labels;
  for (std::size_t i = 0; i < pet.foods.size(); ++i) {
    const sf::Sprite& sprite = icons.sprites[i];
    labels.push_back(makeText(font, layout.labelSize, sf::Color::Black, sprite.getPosition().x,
                              sprite.getPosition().y + sprite.getGlobalBounds().height + 10,
                              std::string(pet.foods[i].name) + " - " +
                                  std::to_string(pet.foods[i].price) + " coins"));
  }

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type != sf::Event::MouseButtonPressed) continue;
      sf::Vector2f click = toScene(window, event.mouseButton.x, event.mouseButton.y);
      noCoinsText.setString("");
      for (std::size_t i = 0; i < pet.foods.size(); ++i) {
        if (!icons.sprites[i].getGlobalBounds().contains(click)) continue;
        const Food& food = pet.foods[i];
        if (coins >= food.price) {
          coins -= food.price;
          pantry[food.name]++;
          trolleyCount++;
          coinsText.setString("Coins: " + std::to_string(coins));
          trolleyText.setString("Trolley: " + std::to_string(trolleyCount));
        } else {
          noCoinsText.setString(layout.noCoinsText);
        }
      }
    }

    clearScene(window, sf::Color(230, 230, 220));
    window.draw(coinsText);
    window.draw(trolleyText);
    window.draw(trolleySprite);
    for (std::size_t i = 0; i < pet.foods.size(); ++i) {
      window.draw(icons.sprites[i]);
      window.draw(labels[i]);
    }
    window.draw(noCoinsText);
    window.display();
  }
  return coins;
}
