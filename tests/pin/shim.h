// Force-included (-include) ahead of every production file in the pin build.
// It swaps the SFML pieces that touch the screen, the wall clock, the mouse and
// the speakers for scripted fakes, so the real game loops run offscreen and
// deterministically without any edit to production code.
#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// Every standard header production uses, included before the macros below so
// the macros only rewrite production code.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace sf {
class PinWindow;
}

namespace pin {
inline sf::Time& now() {
  static sf::Time t;
  return t;
}
inline std::deque<sf::Event>& events() {
  static std::deque<sf::Event> q;
  return q;
}
inline sf::Vector2i& mouse() {
  static sf::Vector2i m;
  return m;
}
// The largest window the screen allows. macOS shrinks a window that does not
// fit to this size when it opens, and SFML's default view shrinks with it.
inline sf::Vector2u& screen() {
  static sf::Vector2u s(1920, 1080);
  return s;
}
inline std::function<void(sf::PinWindow&)>& onDisplay() {
  static std::function<void(sf::PinWindow&)> f;
  return f;
}
// Fixed wall-clock epoch; time() advances with the fake clock.
const std::time_t kEpoch = 1700000000;
}  // namespace pin

namespace sf {

class PinWindow : public RenderTexture {
 public:
  PinWindow() {}
  PinWindow(VideoMode mode, const String& title, Uint32 = Style::Default,
            const ContextSettings& = ContextSettings()) {
    create(mode, title);
  }
  void create(VideoMode mode, const String& title, Uint32 = Style::Default,
              const ContextSettings& = ContextSettings()) {
    if (!RenderTexture::create(std::min(mode.width, pin::screen().x),
                               std::min(mode.height, pin::screen().y)))
      throw std::runtime_error("pin: RenderTexture::create failed");
    title_ = title.toAnsiString();
    open_ = true;
  }
  bool isOpen() const { return open_; }
  void close() { open_ = false; }
  bool pollEvent(Event& event) {
    if (!open_ || pin::events().empty()) return false;
    event = pin::events().front();
    pin::events().pop_front();
    return true;
  }
  void display() {
    RenderTexture::display();
    if (pin::onDisplay()) pin::onDisplay()(*this);
  }
  void setFramerateLimit(unsigned int) {}
  const std::string& title() const { return title_; }

 private:
  std::string title_;
  bool open_ = false;
};

class PinClock {
 public:
  PinClock() : start_(pin::now()) {}
  Time getElapsedTime() const { return pin::now() - start_; }
  Time restart() {
    Time elapsed = getElapsedTime();
    start_ = pin::now();
    return elapsed;
  }

 private:
  Time start_;
};

class PinMouse : public Mouse {
 public:
  static Vector2i getPosition() { return pin::mouse(); }
  static Vector2i getPosition(const PinWindow&) { return pin::mouse(); }
};

// Audio is not pinned; the fake only keeps the "file missing" result real.
class PinMusic {
 public:
  bool openFromFile(const std::string& path) {
    return std::ifstream(path.c_str()).good();
  }
  void play() {}
  void pause() {}
  void stop() {}
  void setLoop(bool) {}
  void setVolume(float) {}
  SoundSource::Status getStatus() const { return SoundSource::Stopped; }
};

}  // namespace sf

inline std::time_t pin_time(std::time_t* out) {
  std::time_t t = pin::kEpoch + static_cast<std::time_t>(pin::now().asSeconds());
  if (out) *out = t;
  return t;
}
namespace std {
using ::pin_time;
}

#define RenderWindow PinWindow
#define Clock PinClock
#define Mouse PinMouse
#define Music PinMusic
#define time(x) pin_time(x)
