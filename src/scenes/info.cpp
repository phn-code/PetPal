#include "scenes.h"

void runInfo(sf::Font& font) {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Information");
  const std::string info =
      "Welcome to the game! Start by choosing your pet: either a ghost "
      "or an avo, and select its age (adult or baby).\n\nNote its "
      "starting stats: health, cleanliness, hunger, and IQ.\n\nPlay "
      "the box game to earn coins by catching falling boxes.\n\nUse "
      "your coins to go grocery shopping—adults can buy chicken, "
      "apples, milk, bread, and medicine, while babies can buy baby "
      "formula, porridge, or fever medicine.\n\nKeep track of your "
      "purchases and use food or medicine by clicking their respective "
      "buttons to boost your pet's stats.\n\nMake sure to shower your "
      "pet at least every 5 minutes for a health and mood "
      "boost.\n\nClick the sleep button when your pet needs rest, "
      "which will update its stats.\n\nCheck your pet's stats with the "
      "stats button; if they drop too low, your pet will need "
      "immediate attention.\n\nEngage in math challenges to increase "
      "IQ.\n\nRemember, if any stat hits zero, a warning will appear, "
      "and your pet won't die, but it will need care.\n\nEnjoy bonding "
      "with your pet, as it will occasionally speak to you throughout "
      "the game!";
  sf::Text text(sf::String::fromUtf8(info.begin(), info.end()), font, 30);
  text.setFillColor(sf::Color::Black);
  text.setPosition(50, 50);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B)
        window.close();
    }
    window.clear(sf::Color(230, 230, 220));
    window.draw(text);
    window.display();
  }
}
