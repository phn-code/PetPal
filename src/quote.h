// The one-liner the pet says on the home screen.
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "catalog.h"

// Seeds rand() with the wall-clock second, so the pick is a function of time.
std::string pickQuote(Species species);

struct Quote {
  std::string text;
  sf::Clock sinceShown;
  bool visible = false;
};

// Picks a new quote every 10 s. `visible` is tracked but drawQuote never reads
// it, so the quote never hides (known bug, phase 3).
void updateQuote(Quote& quote, Species species);

void drawQuote(sf::RenderTarget& target, const sf::Font& font, const std::string& text);
