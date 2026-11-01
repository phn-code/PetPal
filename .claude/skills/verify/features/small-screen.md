# Small screens

On a screen smaller than 1920x1080, such as a 13-inch MacBook, macOS opens each window smaller. Every window shows the whole scene at one scale on both axes, centred, with black bars where the shapes differ. Clicks map through the same scale, and a click in a bar hits nothing.

## Sub-features

- `letterbox` black bars and an undistorted scene in every window.
- `click-map` clicks land on the scaled buttons, edges included.
- `bar-click` a click in a bar does nothing.

## How to get to it (user POV)

- Run the game on a screen under 1920x1080, or resize any window.

## Driving it with the pin

Preconditions: set `pin::screen() = sf::Vector2u(1470, 802)` before `petpal_main()`. Every window then opens at 1470x802, and the scene is 1426x802 with 22 px bars.

- **Click.** Convert scene points with the `px` lambda in `smallScreen`: `x = 22 + scene.x * 802 / 1080`, `y = scene.y * 802 / 1080`.
- **Bars.** `s.inspect` reads pixels of every frame. `smallScreen` checks columns 0, 21, 1448, and 1469.
- **Proof.** `scripts/frames.sh small_screen`, then read any PNG. The real window: `scripts/smoke.sh 4 --screenshot` on a small screen shows the bars in `build/verify/screen.png`.

## Gotchas

- The pin cannot resize a window after it opens. Resizing is covered only by reading `sceneView()` in `src/scene.h`.
- Draw with `clearScene()` and map with `toScene()`. `make lint` rejects anything else.
