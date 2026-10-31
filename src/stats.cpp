#include "stats.h"

#include <algorithm>

void Stats::decay() {
  health = std::max(0, health - 10);
  sleep = std::max(0, sleep - 10);
  hunger = std::max(0, hunger - 10);
  iq = std::max(0, iq - 5);
}

void Stats::addIQ(int score) {
  if (score >= 0 && score <= 5) iq = std::min(100, iq + score * 5);
}
