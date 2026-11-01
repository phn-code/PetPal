// The one-liner the pet says on the home screen.
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include "catalog.h"

// Seeds rand() with the wall-clock second, so the pick is a function of time.
std::string pickQuote(Species species);

struct Quote {
  std::string text;
  bool visible = false;
  sf::Clock sinceChange;  // since the quote last appeared or hid
};

// Hidden for 10 s, then a new quote shows for 10 s, and so on.
void updateQuote(Quote& quote, Species species);

// Draws nothing while the quote is hidden.
void drawQuote(sf::RenderTarget& target, const sf::Font& font, const Quote& quote);
