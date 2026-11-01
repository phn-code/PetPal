#include "quote.h"

#include "scene.h"

#include <cstdlib>
#include <ctime>
#include <vector>

namespace {

const float kNewQuoteInterval = 10.f;
const float kDisplayDuration = 10.f;

const std::vector<std::string> kAvoQuotes = {
    "What did the avocado say to the toast? You’re my butter half!",
    "If life gives you avocados, make guacamole!",
    "I’m just an avocado, living in a toast world.",
    "I’m not a regular avocado, I’m a cool avocado.",
    "You guac my world!",
    "An avocado a day keeps the bad vibes away.",
    "How does an avocado propose? With a guac ring!",
    "Just avocado-ing with the flow!"};

const std::vector<std::string> kGhostQuotes = {
    "I don’t mean to scare you, but I am just here for the boos!",
    "Ghosts are just like us—constantly looking for a good haunt!",
    "If you think I'm scary now, you should see me before my coffee!",
    "What do you call a ghost that tells jokes? A pun-derworld spirit!",
    "Boo! Just kidding. I’m just here for the snacks.",
    "Why dont ghosts like rain? Because it dampens their spirits!"};

}  // namespace

std::string pickQuote(Species species) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  const std::vector<std::string>& quotes = species == Species::Avo ? kAvoQuotes : kGhostQuotes;
  return quotes[std::rand() % quotes.size()];
}

void updateQuote(Quote& quote, Species species) {
  float elapsed = quote.sinceChange.getElapsedTime().asSeconds();
  if (!quote.visible && elapsed >= kNewQuoteInterval) {
    quote.text = pickQuote(species);
    quote.visible = true;
    quote.sinceChange.restart();
  } else if (quote.visible && elapsed >= kDisplayDuration) {
    quote.visible = false;
    quote.sinceChange.restart();
  }
}

void drawQuote(sf::RenderTarget& target, const sf::Font& font, const Quote& quote) {
  if (!quote.visible) return;
  sf::Text quoteText(sf::String::fromUtf8(quote.text.begin(), quote.text.end()), font, 39);
  quoteText.setFillColor(sf::Color::Black);
  sf::FloatRect bounds = quoteText.getLocalBounds();
  quoteText.setOrigin(bounds.left + bounds.width / 2, 0);
  quoteText.setPosition(kScene.x / 2, 280);
  target.draw(quoteText);
}
