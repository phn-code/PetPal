#include "boxes.h"

#include <cstdlib>
#include <optional>
#include <string>

#include "../scenes/scenes.h"

namespace {

const sf::Color kBackground(230, 230, 220);
const float kTrackY = 762;  // a box whose bottom reaches the track ends the game
const float kMinHeight = 50, kMaxHeight = 200, kBoxWidth = 50;
const int kSpawnFrames = 10;  // frames between a catch and the next box
const int kCoinsPerPoint = 5;
const char* const kSongs[] = {"assets/audio/songs/call-me-maybe.mp3",
                              "assets/audio/songs/cheap-thrills.mp3",
                              "assets/audio/songs/counting-stars.mp3",
                              "assets/audio/songs/work-from-home.mp3"};

// Boxes shorter than `below` score `points`; the first matching row wins.
struct PointsRow {
  float below;
  int points;
};
const PointsRow kPoints[] = {{60, 6}, {80, 5}, {100, 4}, {120, 3}, {150, 2}, {200, 1}};

const char* const kIntro =
    "Welcome to PetPal!\n\n"
    "Instructions:\n\n"
    "1. Use the slider to catch the falling boxes.\n\n"
    "2. Different box sizes give different points.\n\n"
    "3. Avoid letting the boxes touch the ground!\n\n"
    "4. Press Enter to start the game.\n\n"
    "5. Press Escape at any time to exit.\n\n\n"
    "Good luck!";

enum class Phase { Intro, Playing, Over };

float randomHeight() {
  return kMinHeight + static_cast<float>(std::rand()) / RAND_MAX * (kMaxHeight - kMinHeight);
}

sf::RectangleShape spawnBox() {
  sf::RectangleShape box(sf::Vector2f(kBoxWidth, randomHeight()));
  box.setPosition(static_cast<float>(std::rand() % static_cast<int>(kScene.x - kBoxWidth)), 0);
  box.setFillColor(sf::Color(0, 0, 128));
  box.setOutlineColor(sf::Color::White);
  box.setOutlineThickness(1);
  return box;
}

}  // namespace

int pointsFor(float height) {
  for (const PointsRow& row : kPoints)
    if (height < row.below) return row.points;
  return 0;
}

int runBoxes(sf::Font& font) {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "PetPal",
                          sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
  window.setFramerateLimit(60);
  // The original game drew one height here and never used it. Keeping the
  // draw keeps every box after it the size the goldens recorded.
  randomHeight();

  sf::Texture knobTexture;
  knobTexture.loadFromFile("assets/minigames/boxes-character.png");
  sf::Sprite knob(knobTexture);
  knob.setScale(98.6f / knobTexture.getSize().x,
                (98.6f * static_cast<float>(knobTexture.getSize().y) / knobTexture.getSize().x) /
                    knobTexture.getSize().y);
  knob.setPosition(810, 700);
  sf::RectangleShape track(sf::Vector2f(kScene.x, 5));
  track.setPosition(0, kTrackY);
  track.setFillColor(sf::Color::Black);

  sf::Music music;
  music.openFromFile(kSongs[std::rand() % std::size(kSongs)]);
  music.setLoop(true);
  music.setVolume(50);
  music.play();
  sf::Music gameOverSound;

  Phase phase = Phase::Intro;
  std::optional<sf::RectangleShape> box;
  int spawnTimer = kSpawnFrames;
  float speed = 3;
  bool dragging = false;
  int points = 0;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) window.close();
        if (phase == Phase::Intro && event.key.code == sf::Keyboard::Enter) phase = Phase::Playing;
      }
      if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left &&
          knob.getGlobalBounds().contains(toScene(window, event.mouseButton.x, event.mouseButton.y)))
        dragging = true;
      if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        dragging = false;
    }

    if (phase == Phase::Playing) {
      if (!box) {
        if (spawnTimer >= kSpawnFrames) {
          box = spawnBox();
          spawnTimer = 0;
        } else {
          ++spawnTimer;
        }
      }
      if (box) {
        box->move(0, speed);
        if (box->getPosition().y + box->getSize().y >= kTrackY) {
          phase = Phase::Over;
          music.pause();
          gameOverSound.openFromFile("assets/audio/boxes-game-over.mp3");
          gameOverSound.setVolume(100);
          gameOverSound.play();
        }
      }
      if (dragging) {
        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        float width = knob.getGlobalBounds().width;
        float x = toScene(window, mouse.x, mouse.y).x - width / 2;
        knob.setPosition(std::max(0.f, std::min(x, kScene.x - width)), knob.getPosition().y);
      }
      if (box && knob.getGlobalBounds().intersects(box->getGlobalBounds())) {
        points += pointsFor(box->getSize().y);
        box.reset();
        speed += 0.5f;
      }
    }

    clearScene(window, kBackground);
    if (phase == Phase::Intro) {
      window.draw(makeText(font, 50, sf::Color::Black, 100, 100, kIntro));
    } else if (phase == Phase::Playing) {
      if (box) window.draw(*box);
      window.draw(track);
      window.draw(knob);
      window.draw(makeText(font, 65, sf::Color::Black, 10, kScene.y - 70,
                           "Points: " + std::to_string(points) + "\n\n"));
    } else {
      window.draw(makeText(font, 80, sf::Color::Black, 550, 300,
                           "  GAME OVER!!\n\nPoints Scored: " + std::to_string(points) +
                               "\nCoins earned: " + std::to_string(points * kCoinsPerPoint) +
                               "\n\nPress esc to Exit\n\n"));
    }
    window.display();
  }
  return points * kCoinsPerPoint;
}
