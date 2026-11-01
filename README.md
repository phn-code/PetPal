# PetPal

PetPal is a virtual-pet game for macOS, written in C++17 with SFML 2. You pick an avocado or a ghost, adult or baby, and keep it fed, clean, rested, and clever. You earn coins in a box-catching game, buy food, and raise IQ in a math game. The game saves to `pet_stats.txt` as you play.

## Install the prerequisites

```sh
brew install sfml@2
```

You also need the Xcode command line tools (`xcode-select --install`) and `python3`.

## Build and run

```sh
make
./build/Petpal
```

Run the game from the repository root. It loads `assets/` by relative path and writes `pet_stats.txt` to the working directory. `make run` builds and runs it the same way.

## Test

```sh
make check        # everything below, in order
make test         # unit tests in tests/unit
make pin          # replays scripted play and compares every frame with tests/golden
make check-assets # every asset the code names is tracked, and every tracked asset is used
make lint         # structural rules in tests/lint.py
```

`tests/pin/README.md` explains the pin and how to update its goldens after an intended change.

## Layout

- `src/main.cpp` opens the main window and runs the menu, the pet picker, and the home screen.
- `src/catalog.cpp` and `src/catalog.h` hold everything that differs between the four pets as a `PetSpec` table, plus each species' sound and quotes.
- `src/scene.h` scales the 1920x1080 scene to any window size and maps clicks back into it.
- `src/scenes/` has one file per window: start menu, pet picker, home, shop, fridge, and info.
- `src/minigames/` has the box-catching game (`boxes.cpp`) and the math game (`math.cpp`).
- `src/stats.cpp` holds the stats and their decay. `src/save.cpp` reads and writes the save file.
- `src/anim.cpp` plays a sprite sheet. `src/quote.cpp` picks and shows the pet's quotes.
- `assets/` holds every image, font, and sound.
- `tests/unit/`, `tests/pin/`, and `tests/golden/` hold the unit tests, the pin, and its goldens.
- `tests/check_assets.py` and `tests/lint.py` are the asset check and the lint.
- `.claude/skills/verify/` tells a coding agent how to prove a change in the running game.
