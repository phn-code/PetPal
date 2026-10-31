#include "anim.h"

#include <iostream>

namespace {
const sf::Vector2f kPetPosition(600.f, 350.f);
const float kPetScale = 8.f;
}  // namespace

SheetPlayer::SheetPlayer(const Sheet& sheet, Activity activity)
    : sheet_(sheet), activity_(activity) {
  if (!texture_.loadFromFile(sheet.path))
    std::cerr << "Error loading sprite sheet: " << sheet.path << std::endl;
  sprite_.setTexture(texture_);
  sprite_.setScale(kPetScale, kPetScale);
  show(0);
}

// Eating holds the first frame for the full duration, then flicks through the
// other two at half speed while the pet leans 3 px into its food.
float SheetPlayer::frameSeconds(int frame) const {
  return activity_ == Activity::Eat && frame > 0 ? sheet_.frameSeconds / 2
                                                 : sheet_.frameSeconds;
}

float SheetPlayer::nudgeX(int frame) const {
  return activity_ == Activity::Eat && frame > 0 ? -3.f : 0.f;
}

void SheetPlayer::show(int frame) {
  frame_ = frame;
  sprite_.setTextureRect(sf::IntRect((frame % sheet_.cols) * sheet_.cellW,
                                     (frame / sheet_.cols) * sheet_.cellH,
                                     sheet_.cellW, sheet_.cellH));
  sprite_.setPosition(kPetPosition.x + nudgeX(frame), kPetPosition.y);
}

void SheetPlayer::update(float dt) {
  elapsed_ += dt;
  if (elapsed_ >= frameSeconds(frame_)) {
    elapsed_ = 0.f;
    show((frame_ + 1) % sheet_.frames);
  } else {
    show(frame_);
  }
}
