#ifndef QUOTE_FILE_H
#define QUOTE_FILE_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "adult_avo.h"
#include "adult_ghost.h"
#include "baby_avo.h"
#include "baby_ghost.h"
#include "pet_stats.h"
sf::Clock quote_clock;
float displayDuration = 10.0f;
float newQuoteInterval = 10.0f;
bool quoteVisible = false;
std::string currentQuote;
std::string petType;
// Define the quotes for Avocado and Ghost
std::vector<std::string> avocado_quotes = {

    "What did the avocado say to the toast? You’re my butter half!",
    "If life gives you avocados, make guacamole!",
    "I’m just an avocado, living in a toast world.",
    "I’m not a regular avocado, I’m a cool avocado.",
    "You guac my world!",
    "An avocado a day keeps the bad vibes away.",
    "How does an avocado propose? With a guac ring!",
    "Just avocado-ing with the flow!"};

std::vector<std::string> ghost_quotes = {
    "I don’t mean to scare you, but I am just here for the boos!",
    "Ghosts are just like us—constantly looking for a good haunt!",
    "If you think I'm scary now, you should see me before my coffee!",
    "What do you call a ghost that tells jokes? A pun-derworld spirit!",
    "Boo! Just kidding. I’m just here for the snacks.",
    "Why dont ghosts like rain? Because it dampens their spirits!"};

std::string getRandomQuote(const std::string& petType) {
  std::srand(static_cast<unsigned>(
      std::time(0)));  // Seed for random number generation
  if (petType == "adult_avo" || petType == "baby_avo") {
    int index = std::rand() % avocado_quotes.size();
    return avocado_quotes[index];
  } else if (petType == "adult_ghost" || petType == "baby_ghost") {
    int index = std::rand() % ghost_quotes.size();
    return ghost_quotes[index];
  }
  return "Null";
}
void updateQuote(std::string& selectedQuote, sf::Clock& quoteClock,
                 float quoteInterval, std::string& petType,
                 bool& quoteVisible) {
  if (quoteClock.getElapsedTime().asSeconds() >= quoteInterval) {
    selectedQuote = getRandomQuote(petType);
    quoteClock.restart();
    quoteVisible = true;
  }

  if (quoteVisible &&
      quoteClock.getElapsedTime().asSeconds() >= displayDuration) {
    quoteVisible = false;
  }
}

void renderQuote(sf::RenderWindow& window, sf::Font& font, std::string& quote) {
  sf::Text quoteText;
  quoteText.setFont(font);
  quoteText.setCharacterSize(39);
  quoteText.setFillColor(sf::Color::Black);
  quoteText.setString(quote);
  sf::FloatRect textBounds = quoteText.getLocalBounds();
  quoteText.setOrigin(textBounds.left + textBounds.width / 2, 0);
  quoteText.setPosition(window.getSize().x / 2, 280);
  window.draw(quoteText);
}

#endif