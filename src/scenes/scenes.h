// Each scene owns its window loop and returns when that window closes.
#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

#include "../catalog.h"
#include "../../pet_stats.h"

// Food name -> count bought. Written to the save file after every home frame.
using Basket = std::map<std::string, int>;

// The pet's foods as 150 px icons placed on the grid. Textures stay put so
// the sprites can point at them.
struct FoodIcons {
  std::vector<sf::Texture> textures;
  std::vector<sf::Sprite> sprites;
};
FoodIcons loadFoodIcons(const std::vector<Food>& foods, const Grid& grid);

inline sf::Text makeText(const sf::Font& font, unsigned size, sf::Color color, float x, float y,
                         const std::string& text = "") {
  sf::Text t(text, font, size);
  t.setFillColor(color);
  t.setPosition(x, y);
  return t;
}

// Returns the coins left after shopping.
int runShop(const PetSpec& pet, sf::Font& font, int coins, Basket& basket);

// Eating from what the save file says was bought. Feeding fills hunger.
void runFridge(const PetSpec& pet, sf::Font& font, PetStats& stats);
