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

int FoodIcons::hit(sf::Vector2f point) const {
  for (std::size_t i = 0; i < sprites.size(); ++i)
    if (sprites[i].getGlobalBounds().contains(point)) return static_cast<int>(i);
  return -1;
}

sf::Vector2f FoodIcons::labelAt(std::size_t i, float gap) const {
  sf::FloatRect icon = sprites[i].getGlobalBounds();
  return {sprites[i].getPosition().x, sprites[i].getPosition().y + icon.height + gap};
}
