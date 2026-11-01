// The math game. runMath() in scenes/scenes.h plays it.
#pragma once

#include <limits>
#include <optional>
#include <string>

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
