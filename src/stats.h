// The five numbers the game tracks. Rules live here; drawing does not.
#pragma once

struct Stats {
  int health = 25, sleep = 25, hunger = 25, iq = 50, money = 1000;

  // One tick: health, sleep and hunger lose 10, IQ loses 5, nothing below 0.
  void decay();
  // 0 to 5 right answers add 5 IQ each, capped at 100. Other scores do nothing.
  void addIQ(int score);
};
