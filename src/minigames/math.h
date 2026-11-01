#ifndef MATHGAME_H
#define MATHGAME_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

// The answer being typed: a sign and at most as many digits as any int holds,
// so every state converts to a number.
class AnswerInput {
 public:
  void toggleSign() { negative_ = !negative_; }
  void addDigit(int digit) {
    if (digits_.size() < kMaxDigits) digits_ += static_cast<char>('0' + digit);
  }
  // Deletes the last digit, or the sign once no digits are left.
  void erase() {
    if (digits_.empty())
      negative_ = false;
    else
      digits_.pop_back();
  }
  // Empty until at least one digit is typed.
  std::optional<int> value() const {
    if (digits_.empty()) return std::nullopt;
    int magnitude = std::stoi(digits_);
    return negative_ ? -magnitude : magnitude;
  }
  std::string text() const { return (negative_ ? "-" : "") + digits_; }

 private:
  static constexpr std::size_t kMaxDigits = std::numeric_limits<int>::digits10;
  bool negative_ = false;
  std::string digits_;
};

class math_game {
 private:
  sf::RenderWindow* game_window;
  sf::Event ev;
  int correct_answers;

  // store current question and answer:
  std::string current_question;
  int current_answer;
  AnswerInput current_input;
  int number_of_answers;

  void generate_question();
  void display_intro();
  void rules_page();
  void display_results();  // display how many question they got correct and
                           // coins gained as a result

  // time keeping for clock
  sf::Clock timer;
  double total_time;
  sf::Music math_audio;
  sf::Music wrong_audio;

 public:
  math_game();  // constructor to initialize game window
  ~math_game();
  void run();
  void render();
  int getScore();
};
#endif