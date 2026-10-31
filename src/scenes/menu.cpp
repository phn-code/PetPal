#include <fstream>

#include "scenes.h"

bool runMenu(sf::RenderWindow& window, sf::Font& font) {
  sf::Vector2u size = window.getSize();

  sf::Text newGame("New Game", font, 70);
  newGame.setFillColor(sf::Color::Black);
  sf::FloatRect newGameBounds = newGame.getGlobalBounds();
  float newGameY = (size.y - newGameBounds.height) / 2 - 120;
  newGame.setPosition((size.x - newGameBounds.width) / 2, newGameY);

  sf::Text loadGame("Load Previous Game", font, 70);
  loadGame.setFillColor(sf::Color::Black);
  sf::FloatRect loadGameBounds = loadGame.getGlobalBounds();
  float loadGameY = newGameY + newGameBounds.height + 120;
  loadGame.setPosition((size.x - loadGameBounds.width) / 2, loadGameY);

  sf::Text noGames("No Previous Games Found", font, 30);
  noGames.setFillColor(sf::Color::Red);
  sf::FloatRect noGamesBounds = noGames.getGlobalBounds();
  noGames.setPosition((size.x - noGamesBounds.width) / 2,
                      loadGameY + loadGameBounds.height + 20);
  bool showNoGames = false;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        // Known bug (phase 3): closing here reads as "New Game", and the pet
        // picker then spins on the closed window.
        return true;
      }
      if (event.type == sf::Event::MouseButtonPressed &&
          event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(sf::Mouse::getPosition(window));
        if (newGame.getGlobalBounds().contains(mouse)) return true;
        if (loadGame.getGlobalBounds().contains(mouse)) {
          if (std::ifstream("pet_stats.txt")) return false;
          showNoGames = true;
        }
      }
    }

    window.clear(sf::Color(230, 230, 220));
    window.draw(newGame);
    window.draw(loadGame);
    if (showNoGames) window.draw(noGames);
    window.display();
  }
  return false;
}
