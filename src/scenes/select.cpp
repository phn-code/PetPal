#include <iostream>
#include <optional>

#include "scenes.h"

const PetSpec* runSelect(sf::RenderWindow& window, sf::Font& font) {
  sf::Texture avoTexture, ghostTexture;
  if (!avoTexture.loadFromFile("avoButton.png") || !ghostTexture.loadFromFile("ghostButton.png")) {
    std::cerr << "Button Images Not Found" << std::endl;
    return nullptr;
  }
  sf::Sprite avoButton(avoTexture);
  avoButton.setScale(0.02f, 0.02f);
  avoButton.setPosition(300, 400);
  sf::Sprite ghostButton(ghostTexture);
  ghostButton.setScale(0.2f, 0.2f);
  ghostButton.setPosition(900, 400);

  sf::Text title("Choose a Pet :)", font, 80);
  title.setFillColor(sf::Color::Black);
  title.setPosition((window.getSize().x - title.getGlobalBounds().width) / 2, 50);
  sf::Text avoText = makeText(font, 50, sf::Color::Black, 300, 560, "Avo");
  sf::Text ghostText = makeText(font, 50, sf::Color::Black, 900, 560, "Ghost");
  sf::Text adultText =
      makeText(font, 40, sf::Color::Black, (window.getSize().x / 2) - 100, 700, "Adult");
  sf::Text babyText =
      makeText(font, 40, sf::Color::Black, (window.getSize().x / 2) + 50, 700, "Baby");

  std::optional<Species> species;
  // Keeps drawing after the window closes (known bug, phase 3; see runMenu).
  for (;;) {
    window.clear(sf::Color(230, 230, 220));
    window.draw(avoButton);
    window.draw(ghostButton);
    window.draw(title);
    window.draw(avoText);
    window.draw(ghostText);
    if (species) {
      window.draw(adultText);
      window.draw(babyText);
    }
    window.display();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        return nullptr;
      }
      if (event.type != sf::Event::MouseButtonPressed) continue;
      sf::Vector2i click = sf::Mouse::getPosition(window);
      if (avoButton.getGlobalBounds().contains(click.x, click.y)) {
        species = Species::Avo;
      } else if (ghostButton.getGlobalBounds().contains(click.x, click.y)) {
        species = Species::Ghost;
      } else if (species && adultText.getGlobalBounds().contains(click.x, click.y)) {
        return &findPet(*species, Stage::Adult);
      } else if (species && babyText.getGlobalBounds().contains(click.x, click.y)) {
        return &findPet(*species, Stage::Baby);
      }
    }
  }
}
