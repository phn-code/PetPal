// Each scene owns its window loop and returns when that window closes.
#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <vector>

#include "../catalog.h"
#include "../quote.h"
#include "../stats.h"

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

// True for a new game, false to load pet_stats.txt.
bool runMenu(sf::RenderWindow& window, sf::Font& font);

// The pet picker. Null when the window was closed instead.
const PetSpec* runSelect(sf::RenderWindow& window, sf::Font& font);

// The main loop: buttons, stats, quote, activities. Returns main()'s exit code.
int runHome(sf::RenderWindow& window, sf::Font& font, sf::Music& music, const PetSpec& pet,
            Stats& stats, Quote& quote);

// Returns the coins left after shopping.
int runShop(const PetSpec& pet, sf::Font& font, int coins, Basket& basket);

// Eating from what the save file says was bought. Feeding fills hunger.
void runFridge(const PetSpec& pet, sf::Font& font, Stats& stats);

void runInfo(sf::Font& font);
