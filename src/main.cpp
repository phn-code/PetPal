// PetPal. Assets load by relative path, so run from the repository root.
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "catalog.h"
#include "quote.h"
#include "save.h"
#include "scene.h"
#include "scenes/scenes.h"
#include "stats.h"

int main() {
  sf::Font font;
  if (!font.loadFromFile("assets/fonts/Regular.ttf")) {
    std::cerr << "Font Not Found" << std::endl;
    return -1;
  }
  sf::Music music;
  if (!music.openFromFile("assets/audio/background.mp3")) {
    std::cerr << "Music file not found" << std::endl;
    return -1;
  }
  music.setLoop(true);
  music.play();

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "My Virtual Pet");
  window.setFramerateLimit(60);

  // The quote timer runs from launch, so the first quote appears 10 s after the
  // menu did, not 10 s after the pet did.
  Quote quote;
  const PetSpec* pet = nullptr;
  Stats stats;
  Pantry pantry;
  std::optional<SaveFile> save = readSave("pet_stats.txt");
  MenuChoice choice = runMenu(window, font, save.has_value());
  if (choice == MenuChoice::Quit) return 0;
  if (choice == MenuChoice::NewGame) {
    pet = runSelect(window, font);
    window.display();  // the picker's last frame shows twice, as pinned
    if (!pet) return 0;
  } else {
    pet = save->pet;
    stats = save->stats;
    pantry = save->pantry;
  }
  return runHome(window, font, music, *pet, stats, pantry, quote);
}
