# Care for the pet

The home screen shows four stat bars and coins, the pet, and eight buttons. Sleep, shower, and medicine each refill a stat and play a 6 s animation. Clicking the pet plays its sound. Every 30 s each stat drops, and a stat at 0 shows a red warning.

## Sub-features

- `sleep`, `shower`, `medicine` refill sleep or health and play the activity's sheet.
- `touch` plays the pet sound (not pinned; audio is fake).
- `decay` drops the stats every 30 s.
- `warnings` shows one line per stat at 0.
- `quote` shows a pet quote for 10 s every 20 s.

## How to get to it (user POV)

- Start or load a game. The buttons sit on the home screen.

## Driving it with the pin

Preconditions: a new game started with `startNewGame`.

- **Activity.** `s.click(kMain, kSleep.x, kSleep.y, "sleep", 2)`. The next 48 frames are the animation. Queue the next home click with `kAfterAnimation` as its wait.
- **Decay.** `s.advance(kMain, sf::seconds(30), "30 s later")`, then read the stat bars in the next frame or the save.
- **Proof.** `journey_*` pins every activity frame and the save. `save_writes` checks the saved stats after each activity.

## Gotchas

- During an animation the game reads and drops every event. A click queued too early is lost and the script stalls with `display budget exhausted`.
- `load_game` loads all stats at 0 to show the warnings.
