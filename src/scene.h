// Every window draws a 1920x1080 scene. The OS can open a window smaller than
// that (macOS fits it to the screen) and the player can resize it, so each
// frame scales the scene by one factor on both axes to fit the window, centres
// it, and leaves black bars where the window's shape differs. Clicks arrive in
// window pixels and map back through the same view, so a click in a bar lands
// outside the scene and hits nothing.
#pragma once

#include <SFML/Graphics.hpp>
#include <algorithm>

const sf::Vector2f kScene(1920.f, 1080.f);

inline sf::View sceneView(sf::Vector2u window) {
  const float scale = std::min(window.x / kScene.x, window.y / kScene.y);
  const float width = kScene.x * scale / window.x, height = kScene.y * scale / window.y;
  sf::View view(sf::FloatRect(0.f, 0.f, kScene.x, kScene.y));
  view.setViewport(sf::FloatRect((1 - width) / 2, (1 - height) / 2, width, height));
  return view;
}

// Starts a frame: black bars, then the scene's background.
inline void clearScene(sf::RenderTarget& target, sf::Color background) {
  target.setView(sceneView(target.getSize()));
  target.clear(sf::Color::Black);
  sf::RectangleShape scene(kScene);
  scene.setFillColor(background);
  target.draw(scene);
}

inline sf::Vector2f toScene(const sf::RenderTarget& target, int pixelX, int pixelY) {
  return target.mapPixelToCoords(sf::Vector2i(pixelX, pixelY), sceneView(target.getSize()));
}
