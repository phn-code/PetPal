# PetPal verification map

This map lists what a player can do in PetPal and how to prove each part with the pin. Read it before you drive the game, then use the matching feature file as the recipe.

## Baseline preconditions

- `make check` passes on the base commit.
- Each pin scenario runs in its own sandbox with no `pet_stats.txt` unless the scenario writes one first.
- The fake clock advances 1/8 s per displayed frame, so 8 frames are 1 s of game time.

## Driving conventions

- Drive through `tests/pin/pin.cpp` scenarios and look at frames with `scripts/frames.sh <scenario>`.
- Use the named button constants in `pin.cpp`, not new coordinates, where one exists.
- Prove a click landed by the next window that opens or by the save, not by the frame count alone.

## Features

- [Start a game](./start-game.md) covers the start menu, the pet picker, and loading a save.
- [Care for the pet](./care.md) covers sleep, shower, medicine, touch, stat decay, and warnings.
- [Shop and eat](./shop-and-eat.md) covers buying food and eating it from the fridge.
- [Minigames](./minigames.md) covers the math game and the box game.
- [Small screens](./small-screen.md) covers letterboxing and click mapping in a window smaller than 1920x1080.
