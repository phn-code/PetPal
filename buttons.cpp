#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <string>

#include "src/minigames/math.h"
#include "src/scenes/scenes.h"
#include "avo.h"
#include "ghost.h"
#include "src/minigames/boxes.h"
#include "pet_stats.h"
#include "quotesFile.h"
#include "sprite_loader.h"
#include <fstream>
#include <string>
#include <ostream>



int main() {
  sf::Font font;
  if (!font.loadFromFile("Regular.ttf")) {
    std::cout << "Font Not Found" << std::endl;
    return -1;
  }
  sf::Music backgroundMusic;
  if (!backgroundMusic.openFromFile("main_background_music.mp3")) {
    std::cout << "Music file not found" << std::endl;
    return -1;
  }
  backgroundMusic.setLoop(true);
  backgroundMusic.play();

  // creating main window
  sf::RenderWindow main_window(sf::VideoMode(1920, 1080), "My Virtual Pet");
  main_window.setFramerateLimit(60);
  bool new_game = SpriteLoader::showGameMenu(main_window, font);
  std::cout << new_game << std::endl;
  std::string user_pet;
  std::vector<int> stats;
  if (!new_game) {
    std::ifstream statsFile("pet_stats.txt");  // Open the file for reading
    std::string line;
    if (std::getline(statsFile, line)) {
      std::istringstream iss(line);
      // Get the first word
      if (!(iss >> user_pet)) {
        std::cerr << "Failed to read the first word from the file."
                  << std::endl;
      }
    }
    stats = SpriteLoader::readPetStats("pet_stats.txt");
  } else {
    user_pet = SpriteLoader::show_intro_screen(main_window, font);
    main_window.display();
  }
  std::cout << "User selected pet: " << user_pet << std::endl;
  PetStats* current_user_pet = SpriteLoader::initialize_pet(user_pet);
  const PetSpec* pet_spec = findPet(user_pet);
  PetStats petStats;
  if (!new_game) {
    petStats.setHealthLevel(stats[0]);
    petStats.setSleepLevel(stats[1]);
    petStats.setHungerLevel(stats[2]);
    petStats.setIQLevel(stats[3]);
    petStats.setTotalMoney(stats[4]);
  }
  Basket basket;
  SpriteLoader* current_pet = new SpriteLoader;

  // creating buttons
  if (!SpriteLoader::loadResources()) {
    std::cout << "error loading sprites" << std::endl;
  }

  std::string selectedQuote = getRandomQuote(user_pet);
  // main loop of game
  while (main_window.isOpen()) {
    sf::Event event;
    petStats.updateStats(main_window, font);

    updateQuote(selectedQuote, quote_clock, newQuoteInterval, user_pet,quoteVisible);

    while (main_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        main_window.close();
        return 0;
      }

      // checking if user clicked the icon
      if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i click_position = sf::Mouse::getPosition(main_window);
        int x = click_position.x;
        int y = click_position.y;
        if (x >= 40 && x <= 190 && y >= 130 && y <= 280) {
          std::cout << "Sleeping Button" << std::endl;
          petStats.maxSleep();
          current_pet->animate(*pet_spec, Activity::Sleep, main_window, font, selectedQuote, &petStats);
        } else if (x >= 470 && x <= 620 && y >= 130 && y <= 280) {
          std::cout << "showering Button" << std::endl;
          petStats.maxHealth();
          current_pet->animate(*pet_spec, Activity::Shower, main_window, font, selectedQuote, &petStats);
        } else if (x >= 900 && x <= 1050 && y >= 130 && y <= 280) {
          std::cout << "Game Button" << std::endl;
          backgroundMusic.pause();
          mini_game new_game;
          while (new_game.window_open()) {
            new_game.update();
            new_game.render();
          }
          petStats.changeMoney(new_game.money_earned_end());
          backgroundMusic.play();
        } else if (x >= 1310 && x <= 1460 && y >= 130 && y <= 280) {
          std::cout << "Eating Button" << std::endl;
          runFridge(*pet_spec, font, petStats);
          current_pet->animate(*pet_spec, Activity::Eat, main_window, font, selectedQuote, &petStats);

        } else if (x >= 40 && x <= 190 && y >= 680 && y <= 830) {
          std::cout << "Medicine Button" << std::endl;
          petStats.maxHealth();
          current_pet->animate(*pet_spec, Activity::Medicine, main_window, font, selectedQuote, &petStats);

        } else if (x >= 470 && x <= 620 && y >= 680 && y <= 830) {
          petStats.setTotalMoney(runShop(*pet_spec, font, petStats.getMoney(), basket));
        } else if (x >= 900 && x <= 1050 && y >= 680 && y <= 830) {
          backgroundMusic.pause();
          std::cout << "Math Button" << std::endl;
          math_game test;
          petStats.increaseIQ(test.getScore());
          // petStats.changeIQ(test);
          backgroundMusic.play();
        } else if (x >= 1310 && x <= 1460 && y >= 680 && y <= 830) {
          std::cout << "Info Button" << std::endl;
          // Create a new window for the info screen
          sf::RenderWindow infoWindow(sf::VideoMode(1920, 1080), "Information");
          sf::Text infoText(
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
              "the game!",
              font, 30);
          infoText.setFillColor(sf::Color::Black);
          infoText.setPosition(50, 50);

          // Main loop for the info window
          while (infoWindow.isOpen()) {
            sf::Event infoEvent;
            while (infoWindow.pollEvent(infoEvent)) {
              if (infoEvent.type == sf::Event::Closed) {
                infoWindow.close();
              }
              if (infoEvent.type == sf::Event::KeyPressed &&
                  infoEvent.key.code == sf::Keyboard::B) {
                infoWindow.close();  // Close the info window
              }
            }

            infoWindow.clear(sf::Color(230, 230, 220));  // Clear the window
            infoWindow.draw(infoText);                   // Draw the info text
            infoWindow.display();                        // Display the content
          }
        } else if (x >= 650 && x <= 900 && y >= 400 && y <= 600) {
          std::cout << "Touch character" << std::endl;
          current_user_pet->make_sound();
        }
      }
    }
    current_pet->drawSprites(main_window);
    petStats.renderStats(main_window, font);
    renderQuote(main_window, font, selectedQuote);
    //polymorphism
    current_pet->draw_default_sprite(main_window,user_pet);
    petStats.checkStats(main_window, font);
    main_window.display();

    // writting data to file:
    std::ofstream statsFile("pet_stats.txt");
    statsFile << user_pet << " " << petStats.getHealthLevel() << " "
              << petStats.getSleepLevel() << " " << petStats.getHungerLevel()
              << " " << petStats.getIQLevel() << " " << petStats.getTotalMoney()
              << "\n"
              << std::endl;
    // Iterate through the basket and write item details to the file
    statsFile.flush();
    for (const auto& entry : basket) {
      statsFile << entry.first << " " << entry.second << "\n";
    }
    }
  return 0;
}