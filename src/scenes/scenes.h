// Each scene owns its window loop and returns when that window closes.
#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

#include "../catalog.h"

// Food name -> count bought. Written to the save file after every home frame.
using Basket = std::map<std::string, int>;

// The pet's foods as 150 px icons placed on the grid. Textures stay put so
// the sprites can point at them.
struct FoodIcons {
  std::vector<sf::Texture> textures;
  std::vector<sf::Sprite> sprites;
};
FoodIcons loadFoodIcons(const std::vector<Food>& foods, const Grid& grid);

// Returns the coins left after shopping.
int runShop(const PetSpec& pet, sf::Font& font, int coins, Basket& basket);
