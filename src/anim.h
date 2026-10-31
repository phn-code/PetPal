#pragma once

#include <SFML/Graphics.hpp>

#include "catalog.h"

// Plays one activity's sheet at the pet's spot on the home screen.
class SheetPlayer {
 public:
  SheetPlayer(const Sheet& sheet, Activity activity);

  void update(float dt);
  void draw(sf::RenderTarget& target) const { target.draw(sprite_); }
  int frame() const { return frame_; }
  sf::IntRect rect() const { return sprite_.getTextureRect(); }

 private:
  float frameSeconds(int frame) const;
  float nudgeX(int frame) const;
  void show(int frame);

  Sheet sheet_;
  Activity activity_;
  sf::Texture texture_;
  sf::Sprite sprite_;
  int frame_ = 0;
  float elapsed_ = 0.f;
};
