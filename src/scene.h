// Every window draws a 1920x1080 scene. The OS can open a window smaller than
// that (macOS fits it to the screen) and the player can resize it, so each
// window scales the whole scene to its size, and clicks, which arrive in
// window pixels, are mapped back into the scene.
#pragma once

#include <SFML/Graphics.hpp>

const sf::Vector2f kScene(1920.f, 1080.f);

inline void showWholeScene(sf::RenderWindow& window) {
  window.setView(sf::View(sf::FloatRect(0.f, 0.f, kScene.x, kScene.y)));
}

inline sf::Vector2f toScene(const sf::RenderWindow& window, int pixelX, int pixelY) {
  return window.mapPixelToCoords(sf::Vector2i(pixelX, pixelY));
}
