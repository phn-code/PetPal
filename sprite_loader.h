#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include "avo.h"
#include "ghost.h"
#include "src/anim.h"
#include "src/catalog.h"
#include "pet_stats.h"
#include "quotesFile.h"

class SpriteLoader {
 public:
  static sf::Texture sleeping_button_texture;
  static sf::Sprite sleeping_button_sprite;

  static sf::Texture adult_avo_normal;
  static sf::Sprite adult_avo_normal_sprite;

  static sf::Texture baby_avo_normal;
  static sf::Sprite baby_avo_normal_sprite;

  static sf::Texture adult_ghost_normal;
  static sf::Sprite adult_ghost_normal_sprite;

  static sf::Texture baby_ghost_normal;
  static sf::Sprite baby_ghost_normal_sprite;

  static sf::Texture shower_button_texture;
  static sf::Sprite shower_button_sprite;

  static sf::Texture game_button_texture;
  static sf::Sprite game_button_sprite;

  static sf::Texture food_button_texture;
  static sf::Sprite food_button_sprite;

  static sf::Texture health_button_texture;
  static sf::Sprite health_button_sprite;

  static sf::Texture shopping_button_texture;
  static sf::Sprite shopping_button_sprite;

  static sf::Texture info_button_texture;
  static sf::Sprite info_button_sprite;

  static sf::Texture math_button_texture;
  static sf::Sprite math_button_sprite;
  static sf::RectangleShape line;

  static bool loadResources() {
    line.setSize(sf::Vector2f(1920, 5));
    line.setFillColor(sf::Color::Black);
    line.setPosition(0, 120);
    // Load sleeping button texture
    if (!sleeping_button_texture.loadFromFile("moon.png")) {
      std::cout << "Sleeping button image not found." << std::endl;
      return false;
    }
    sleeping_button_sprite.setTexture(sleeping_button_texture);
    sleeping_button_sprite.setPosition(40, 130);

    // Load adult avo texture
    if (!adult_avo_normal.loadFromFile("bigCharNormalAvo.png")) return false;
    adult_avo_normal_sprite.setTexture(adult_avo_normal);
    adult_avo_normal_sprite.setPosition(600, 350);
    adult_avo_normal_sprite.setScale(8.0f, 8.0f);

    // Load baby avo texture
    if (!baby_avo_normal.loadFromFile("smallAvoNormal.png")) return false;
    baby_avo_normal_sprite.setTexture(baby_avo_normal);
    baby_avo_normal_sprite.setPosition(600, 350);
    baby_avo_normal_sprite.setScale(8.0f, 8.0f);

    // Load adult ghost texture
    if (!adult_ghost_normal.loadFromFile("bigCharNormalGhost.png"))
      return false;
    adult_ghost_normal_sprite.setTexture(adult_ghost_normal);
    adult_ghost_normal_sprite.setPosition(600, 350);
    adult_ghost_normal_sprite.setScale(8.0f, 8.0f);

    // Load baby ghost texture
    if (!baby_ghost_normal.loadFromFile("smallGhostNormal.png")) return false;
    baby_ghost_normal_sprite.setTexture(baby_ghost_normal);
    baby_ghost_normal_sprite.setPosition(600, 350);
    baby_ghost_normal_sprite.setScale(8.0f, 8.0f);

    // Load shower button texture
    if (!shower_button_texture.loadFromFile("shower (1).png")) {
      std::cout << "Shower button image not found." << std::endl;
      return false;
    }
    shower_button_sprite.setTexture(shower_button_texture);
    shower_button_sprite.setPosition(470, 130);

    // Load game button texture
    if (!game_button_texture.loadFromFile("console.png")) {
      std::cout << "Game button image not found." << std::endl;
      return false;
    }
    game_button_sprite.setTexture(game_button_texture);
    game_button_sprite.setPosition(900, 130);

    // Load food button texture
    if (!food_button_texture.loadFromFile("cutlery.png")) {
      std::cout << "Food button image not found." << std::endl;
      return false;
    }
    food_button_sprite.setTexture(food_button_texture);
    food_button_sprite.setPosition(1310, 130);

    // Load health button texture
    if (!health_button_texture.loadFromFile("first-aid-kit.png")) {
      std::cout << "Health button image not found." << std::endl;
      return false;
    }
    health_button_sprite.setTexture(health_button_texture);
    health_button_sprite.setPosition(40, 680);

    // Load shopping button texture
    if (!shopping_button_texture.loadFromFile("grocery-cart.png")) {
      std::cout << "Shopping button image not found." << std::endl;
      return false;
    }
    shopping_button_sprite.setTexture(shopping_button_texture);
    shopping_button_sprite.setPosition(470, 680);

    // Load info button texture
    if (!info_button_texture.loadFromFile("info.png")) {
      std::cout << "Info button image not found." << std::endl;
    }
    info_button_sprite.setTexture(info_button_texture);
    info_button_sprite.setPosition(1310, 680);

    // Load math button texture
    if (!math_button_texture.loadFromFile("math.png")) {
      std::cout << "Math button image not found." << std::endl;
    }
    math_button_sprite.setTexture(math_button_texture);
    math_button_sprite.setPosition(900, 680);

    return true;
  }

  static void drawSprites(sf::RenderWindow& window) {
    window.clear(sf::Color(230, 230, 220));
    window.draw(line);
    window.draw(sleeping_button_sprite);
    window.draw(shower_button_sprite);
    window.draw(game_button_sprite);
    window.draw(food_button_sprite);
    window.draw(health_button_sprite);
    window.draw(shopping_button_sprite);
    window.draw(info_button_sprite);
    window.draw(math_button_sprite);
  }

  static PetStats* initialize_pet(std::string user_pet) {
    if (user_pet.find("avo") != std::string::npos) {
      std::cout << "The pet is an avo!" << std::endl;
      return new avo;
    } else {
      std::cout << "The pet is not an avo." << std::endl;
      return new ghost;
    }
  }

  static std::string show_intro_screen(sf::RenderWindow& window,
                                       sf::Font& font) {
    // Load button textures
    sf::Texture avo_texture, ghost_texture;
    if (!avo_texture.loadFromFile("avoButton.png") ||
        !ghost_texture.loadFromFile("ghostButton.png")) {
      std::cout << "Button Images Not Found" << std::endl;
      return "";
    }
    // Create sprites for the buttons
    sf::Sprite avoButton(avo_texture);
    avoButton.setScale(0.02f, 0.02f);
    avoButton.setPosition(300, 400);  // Adjust position based on button size
    sf::Sprite ghostButton(ghost_texture);
    ghostButton.setScale(0.2f, 0.2f);
    ghostButton.setPosition(900, 400);  // Adjust position based on button size
    // Create text objects
    sf::Text title("Choose a Pet :)", font, 80);  // Title text
    title.setFillColor(sf::Color::Black);
    title.setPosition((window.getSize().x - title.getGlobalBounds().width) / 2,
                      50);  // Center the title

    sf::Text avoText("Avo", font, 50);  // Avo text
    avoText.setFillColor(sf::Color::Black);
    avoText.setPosition(300, 560);  // Position below the Avo button

    sf::Text ghostText("Ghost", font, 50);  // Ghost text
    ghostText.setFillColor(sf::Color::Black);
    ghostText.setPosition(900, 560);  // Position below the Ghost button

    // Text for selecting Adult or Baby
    sf::Text adultText("Adult", font, 40);
    adultText.setFillColor(sf::Color::Black);
    adultText.setPosition((window.getSize().x / 2) - 100,
                          700);  // Center below the options

    sf::Text babyText("Baby", font, 40);
    babyText.setFillColor(sf::Color::Black);
    babyText.setPosition((window.getSize().x / 2) + 50,
                         700);  // Center next to Adult

    bool inIntro = true;      // Track whether we are in the intro screen
    std::string selectedPet;  // To store the selected pet type

    while (inIntro) {
      sf::Event event;

      // Clear the window
      window.clear(sf::Color(230, 230, 220));

      // Draw the buttons
      window.draw(avoButton);
      window.draw(ghostButton);

      // Draw the title and button labels
      window.draw(title);
      window.draw(avoText);
      window.draw(ghostText);

      // If a pet is selected, show the Adult/Baby options
      if (!selectedPet.empty()) {
        window.draw(adultText);
        window.draw(babyText);
      }

      // Display the window contents
      window.display();

      // Event loop to handle user input
      while (window.pollEvent(event)) {
        std::cout << "test" << std::endl;
        if (event.type == sf::Event::Closed) {
          window.close();
          return "";
        }

        // Check if user clicked a button
        if (event.type == sf::Event::MouseButtonPressed) {
          sf::Vector2i click_position = sf::Mouse::getPosition(window);
          int x = click_position.x;
          int y = click_position.y;

          std::cout << "Mouse clicked at: " << x << ", " << y << std::endl;

          if (avoButton.getGlobalBounds().contains(x, y)) {
            std::cout << "Avocado selected!" << std::endl;
            selectedPet = "Avo";  // Set the selected pet type
          } else if (ghostButton.getGlobalBounds().contains(x, y)) {
            std::cout << "Ghost selected!" << std::endl;
            selectedPet = "Ghost";  // Set the selected pet type
          } else if (selectedPet == "Avo" &&
                     adultText.getGlobalBounds().contains(x, y)) {
            std::cout << "Adult Avo selected!" << std::endl;
            return "adult_avo";  // Return adult Avo
          } else if (selectedPet == "Avo" &&
                     babyText.getGlobalBounds().contains(x, y)) {
            std::cout << "Baby Avo selected!" << std::endl;
            return "baby_avo";  // Return baby Avo
          } else if (selectedPet == "Ghost" &&
                     adultText.getGlobalBounds().contains(x, y)) {
            std::cout << "Adult Ghost selected!" << std::endl;
            return "adult_ghost";  // Return adult Ghost
          } else if (selectedPet == "Ghost" &&
                     babyText.getGlobalBounds().contains(x, y)) {
            std::cout << "Baby Ghost selected!" << std::endl;
            return "baby_ghost";  // Return baby Ghost
          }
        }
      }
    }

    return "";  // Default return value
  }

  // Every activity plays its sheet over the home screen for 6 seconds.
  static void animate(const PetSpec& pet, Activity activity,
                      sf::RenderWindow& main_window, sf::Font& font,
                      std::string& quote, PetStats* current_stats) {
    SheetPlayer anim(pet.sheets[static_cast<int>(activity)], activity);
    sf::Clock clock;
    sf::Clock total_animation_time;
    while (main_window.isOpen()) {
      sf::Event event;
      while (main_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) main_window.close();
      }
      anim.update(clock.restart().asSeconds());
      drawSprites(main_window);
      anim.draw(main_window);
      renderQuote(main_window, font, quote);
      current_stats->renderStats(main_window, font);
      main_window.display();
      if (total_animation_time.getElapsedTime().asSeconds() >= 6) break;
    }
  }

  static bool showGameMenu(sf::RenderWindow& parentWindow, sf::Font& font) {
    // Set up menu text
    sf::Text newGameText("New Game", font, 70);
    newGameText.setFillColor(sf::Color::Black);
    sf::Vector2u windowSize = parentWindow.getSize();

    // Get the text size for newGameText
    sf::FloatRect textBoundsNewGame = newGameText.getGlobalBounds();

    // Calculate the center position for newGameText
    float newGamePosX =
        (windowSize.x - textBoundsNewGame.width) / 2;  // Center horizontally
    float newGamePosY = (windowSize.y - textBoundsNewGame.height) / 2 -
                        120;  // Center vertically, offset by 50 for spacing
    newGameText.setPosition(newGamePosX, newGamePosY);

    // Set up load game text
    sf::Text loadGameText("Load Previous Game", font, 70);
    loadGameText.setFillColor(sf::Color::Black);

    // Get the text size for loadGameText
    sf::FloatRect textBoundsLoadGame = loadGameText.getGlobalBounds();

    // Calculate the center position for loadGameText
    float loadGamePosX =
        (windowSize.x - textBoundsLoadGame.width) / 2;  // Center horizontally
    float loadGamePosY = newGamePosY + textBoundsNewGame.height +
                         120;  // Position below newGameText with some spacing
    loadGameText.setPosition(loadGamePosX, loadGamePosY);

    // Text for no previous games found
    sf::Text noGamesText("No Previous Games Found", font, 30);
    noGamesText.setFillColor(sf::Color::Red);

    // Get the text size for noGamesText
    sf::FloatRect textBoundsNoGames = noGamesText.getGlobalBounds();

    // Calculate the position for noGamesText
    float noGamesPosX =
        (windowSize.x - textBoundsNoGames.width) / 2;  // Center horizontally
    float noGamesPosY = loadGamePosY + textBoundsLoadGame.height +
                        20;  // Position below loadGameText with some spacing
    noGamesText.setPosition(noGamesPosX, noGamesPosY);
    bool showNoGamesMessage = false;  // Flag to show the no games message

    // Main loop for the menu window
    while (parentWindow.isOpen()) {
      sf::Event event;
      while (parentWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          parentWindow.close();
          return -1;  // Close event
        }

        if (event.type == sf::Event::MouseButtonPressed) {
          if (event.mouseButton.button == sf::Mouse::Left) {
            // Get the mouse position
            sf::Vector2i mousePos = sf::Mouse::getPosition(parentWindow);
            if (newGameText.getGlobalBounds().contains(
                    static_cast<sf::Vector2f>(mousePos))) {
              return true;  // New Game selected
            } else if (loadGameText.getGlobalBounds().contains(
                           static_cast<sf::Vector2f>(mousePos))) {
              // Check if the pet_stats.txt file exists
              std::ifstream file("pet_stats.txt");
              if (file) {
                return false;  // Load Previous Game selected
              } else {
                showNoGamesMessage = true;  // Show the no games message
              }
            }
          }
        }
      }

      // Clear the window with the specified color
      parentWindow.clear(sf::Color(230, 230, 220));

      // Draw the texts
      parentWindow.draw(newGameText);
      parentWindow.draw(loadGameText);

      // Draw the no games message if needed
      if (showNoGamesMessage) {
        parentWindow.draw(noGamesText);
      }

      // Display the contents of the window
      parentWindow.display();
    }

    return false;  // Default return value (shouldn't reach here)
  }

  static std::vector<int> readPetStats(const std::string& filename) {
    std::ifstream statsFile(filename);
    std::vector<int> petStats;

    if (statsFile.is_open()) {
      std::string line;
      // Read the first line (you can adjust this if you expect multiple lines)
      if (std::getline(statsFile, line)) {
        std::istringstream iss(line);
        std::string userPet;
        int healthLevel, sleepLevel, hungerLevel, iqLevel, totalMoney;

        // Read the values from the line
        iss >> userPet >> healthLevel >> sleepLevel >> hungerLevel >> iqLevel >>
            totalMoney;

        // Store the values in the vector
        petStats.push_back(healthLevel);
        petStats.push_back(sleepLevel);
        petStats.push_back(hungerLevel);
        petStats.push_back(iqLevel);
        petStats.push_back(totalMoney);
      }
      statsFile.close();
    } else {
      std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return petStats;
}

static void draw_default_sprite(sf::RenderWindow& window,std::string user_pet){

if (user_pet == "adult_avo") {
      window.draw(SpriteLoader::adult_avo_normal_sprite);
    } else if (user_pet == "baby_avo") {
      window.draw(SpriteLoader::baby_avo_normal_sprite);
    } else if (user_pet == "adult_ghost") {
      window.draw(SpriteLoader::adult_ghost_normal_sprite);
    } else {
      window.draw(SpriteLoader::baby_ghost_normal_sprite);
    }  }


};

// Definition of static members
sf::Texture SpriteLoader::sleeping_button_texture;
sf::Sprite SpriteLoader::sleeping_button_sprite;

sf::Texture SpriteLoader::adult_avo_normal;
sf::Sprite SpriteLoader::adult_avo_normal_sprite;

sf::Texture SpriteLoader::baby_avo_normal;
sf::Sprite SpriteLoader::baby_avo_normal_sprite;

sf::Texture SpriteLoader::adult_ghost_normal;
sf::Sprite SpriteLoader::adult_ghost_normal_sprite;

sf::Texture SpriteLoader::baby_ghost_normal;
sf::Sprite SpriteLoader::baby_ghost_normal_sprite;

sf::Texture SpriteLoader::shower_button_texture;
sf::Sprite SpriteLoader::shower_button_sprite;

sf::Texture SpriteLoader::game_button_texture;
sf::Sprite SpriteLoader::game_button_sprite;

sf::Texture SpriteLoader::food_button_texture;
sf::Sprite SpriteLoader::food_button_sprite;

sf::Texture SpriteLoader::health_button_texture;
sf::Sprite SpriteLoader::health_button_sprite;

sf::Texture SpriteLoader::shopping_button_texture;
sf::Sprite SpriteLoader::shopping_button_sprite;

sf::Texture SpriteLoader::info_button_texture;
sf::Sprite SpriteLoader::info_button_sprite;

sf::Texture SpriteLoader::math_button_texture;
sf::Sprite SpriteLoader::math_button_sprite;
sf::RectangleShape SpriteLoader::line;

#endif