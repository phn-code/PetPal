#include <iostream>

#include "scenes.h"

int runShop(const PetSpec& pet, sf::Font& font, int coins, Pantry& pantry) {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), std::string(pet.title) + " Shop");

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

  FoodIcons icons = loadFoodIcons(pet.foods, pet.shopGrid);
  std::vector<sf::Text> labels;
  for (std::size_t i = 0; i < pet.foods.size(); ++i) {
    sf::Vector2f at = icons.labelAt(i, 10);
    labels.push_back(makeText(font, pet.shopLabelSize, sf::Color::Black, at.x, at.y,
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
      int hit = icons.hit(click);
      if (hit < 0) continue;
      const Food& food = pet.foods[hit];
      if (coins >= food.price) {
        coins -= food.price;
        pantry[food.name]++;
        trolleyCount++;
        coinsText.setString("Coins: " + std::to_string(coins));
        trolleyText.setString("Trolley: " + std::to_string(trolleyCount));
      } else {
        noCoinsText.setString("NOT ENOUGH COINS");
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
