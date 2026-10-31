#ifndef PETSTATUS_H
#define PETSTATUS_H

#pragma once
#include <SFML/Graphics.hpp>
#include "pet_stats.h"


class PetStats {
 private:
  int health_level;
  int sleep_level;
  int hunger_level;
  int iq_level;
  int total_money;
  sf::Clock stat_timer;
  std::vector<std::string> quotes_ghost;
  std::vector<std::string> quotes_avocado;

 public:
  PetStats();  // Constructor

  void updateStats(sf::RenderWindow& window,
                   sf::Font font);  // Decreases stats every 30 seconds

  void renderStats(sf::RenderWindow& window,
                   sf::Font& font);  // Draw the stats bars

  void increaseHunger(int amount);
  void increaseSleep(int amount);
  void increaseIQ(int score);
  void increaseMoney(int amount);
  int getMoney() const;

  // Potions for hunger and sleep
  void useHungerPotion();
  void useSleepPotion();

  // Function to determine the color based on the stat level (if needed)
  sf::Color getBarColor(int level);

  // functions to max stats
  void maxHealth();
  void maxSleep();
  void maxHunger();
  void changeMoney(int someMoney);

  void checkStats(sf::RenderWindow& window, sf::Font& font);
  virtual void make_sound() {};

  // Getters
  int getHealthLevel() const { return health_level; }
  int getSleepLevel() const { return sleep_level; }
  int getHungerLevel() const { return hunger_level; }
  int getIQLevel() const { return iq_level; }
  int getTotalMoney() const { return total_money; }

    // Setters
    void setHealthLevel(int level) { health_level = level; }
    void setSleepLevel(int level) { sleep_level = level; }
    void setHungerLevel(int level) { hunger_level = level; }
    void setIQLevel(int level) { iq_level = level; }
    void setTotalMoney(int money) { total_money = money; }
    
    //polymorphism
    virtual void draw_default_sprite(sf::RenderWindow& window){}
   

  

};

#endif