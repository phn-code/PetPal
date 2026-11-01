#include "quote.h"

#include "scene.h"

#include <cstdlib>
#include <ctime>
#include <vector>

namespace {

const float kNewQuoteInterval = 10.f;
const float kDisplayDuration = 10.f;

}  // namespace

std::string pickQuote(Species species) {
  std::srand(static_cast<unsigned>(std::time(nullptr)));
  const std::vector<std::string>& quotes = quotesFor(species);
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
