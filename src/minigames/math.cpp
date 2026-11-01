#include "math.h"

#include <cstdlib>
#include <ctime>

#include "../scenes/scenes.h"

namespace {

const sf::Color kPaper(230, 230, 220);
const int kQuestions = 5;
const float kSeconds = 51;  // the clock shows 50 for its first whole second

const char* const kRules =
    "Welcome to PetPal Math Game!\n\n\nInstructions:\n\n1) You have 50 "
    "seconds to answer 5 math questions\n\n2) Type solution and hit enter "
    "to answer\n\n3) Type - for negative\n\n4) Use delete key to remove "
    "entered number\n\n5) Press Enter to Start Game\n\n\nGood Luck!";

// The intro plays a 6-frame sheet for 3 s. Leftover time carries into the
// next frame, unlike the pet animations.
const int kIntroFrames = 6;
const float kIntroFrameSeconds = 0.3f, kIntroSeconds = 3;

enum class Phase { Intro, Rules, Question, Results };

struct Question {
  std::string text;
  int answer;
};

Question randomQuestion() {
  int a = std::rand() % 100;
  int b = std::rand() % 100;
  char op = "+-*"[std::rand() % 3];
  int answer = op == '+' ? a + b : op == '-' ? a - b : a * b;
  return {std::to_string(a) + " " + op + " " + std::to_string(b) + " = ?", answer};
}

sf::Color timerColor(float remaining) {
  if (remaining >= 30) return sf::Color::Black;
  if (remaining > 10) return sf::Color(255, 165, 0);
  return sf::Color::Red;
}

}  // namespace

int runMath(sf::Font& font) {
  sf::Music lobby;
  lobby.openFromFile("assets/audio/math-lobby.mp3");
  lobby.setVolume(100);
  lobby.play();
  sf::Music wrongKey;
  wrongKey.openFromFile("assets/audio/math-wrong.mp3");
  wrongKey.setVolume(100);
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Math Game");
  window.display();

  sf::Font digits;
  digits.loadFromFile("assets/fonts/dogica.ttf");
  sf::Texture introTexture;
  introTexture.loadFromFile("assets/minigames/math-intro.png");
  sf::Sprite intro(introTexture);
  intro.setPosition(280, 120);
  intro.setScale(30, 30);
  const int cellW = introTexture.getSize().x / kIntroFrames, cellH = introTexture.getSize().y;

  Phase phase = Phase::Intro;
  sf::Clock introClock, frameClock, questionClock;
  float frameTime = 0;
  int introFrame = 0;
  Question question;
  AnswerInput input;
  int asked = 0, correct = 0;

  while (window.isOpen()) {
    bool answered = false;
    sf::Event event;
    // Keys after an answer wait for the next question.
    while (!answered && window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) window.close();
      if (event.type != sf::Event::KeyPressed) continue;
      const sf::Keyboard::Key key = event.key.code;
      if (key == sf::Keyboard::Escape) {
        window.close();
      } else if (phase == Phase::Rules && key == sf::Keyboard::Enter) {
        phase = Phase::Question;
        questionClock.restart();
        question = randomQuestion();
      } else if (phase == Phase::Results && key == sf::Keyboard::Enter) {
        window.close();
      } else if (phase == Phase::Question) {
        if (key == sf::Keyboard::Enter) {
          if (std::optional<int> value = input.value()) {
            correct += *value == question.answer;
            answered = true;
          }
        } else if (key == sf::Keyboard::Dash) {
          input.toggleSign();
        } else if (key == sf::Keyboard::BackSpace) {
          input.erase();
        } else if (key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) {
          input.addDigit(key - sf::Keyboard::Num0);
        } else if (wrongKey.getStatus() != sf::SoundSource::Playing) {
          wrongKey.play();
        }
      }
    }

    const float remaining = kSeconds - questionClock.getElapsedTime().asSeconds();
    if (phase == Phase::Intro) {
      frameTime += frameClock.restart().asSeconds();
      if (frameTime >= kIntroFrameSeconds) {
        frameTime -= kIntroFrameSeconds;
        introFrame = (introFrame + 1) % kIntroFrames;
      }
      intro.setTextureRect(sf::IntRect(introFrame * cellW, 0, cellW, cellH));
      clearScene(window, sf::Color(196, 164, 132));
      window.draw(intro);
    } else if (phase == Phase::Rules) {
      clearScene(window, kPaper);
      window.draw(makeText(font, 35, sf::Color::Black, 30, 30, kRules));
    } else if (phase == Phase::Question) {
      clearScene(window, kPaper);
      window.draw(makeText(digits, 55, sf::Color::Black, 480, 300, question.text));
      window.draw(makeText(digits, 40, sf::Color::Black, 680, 400, input.text()));
      window.draw(makeText(digits, 40, sf::Color::Black, 20, 20, "Score: " + std::to_string(correct)));
      window.draw(makeText(digits, 40, timerColor(remaining), 700, 100,
                           std::to_string(static_cast<int>(remaining))));
    } else {
      clearScene(window, kPaper);
      window.draw(makeText(font, 70, sf::Color::Black, 550, 300,
                           "  GAME OVER!!\n\nPoints Scored: " + std::to_string(correct) +
                               "\n\nPress ESC To Exit"));
    }
    window.display();

    if (phase == Phase::Intro && introClock.getElapsedTime().asSeconds() >= kIntroSeconds) {
      phase = Phase::Rules;
    } else if (phase == Phase::Question && remaining <= 0) {
      phase = Phase::Results;
    } else if (answered) {
      // The answered question stays up for one frame with the new score.
      if (++asked == kQuestions) {
        phase = Phase::Results;
      } else {
        question = randomQuestion();
        input = AnswerInput();
      }
    }
  }
  lobby.pause();
  return correct;
}
