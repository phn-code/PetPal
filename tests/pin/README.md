# Characterization pin

The pin records what the game does today, bugs included, so a refactor can prove that it changed nothing.

```sh
make pin                      # compare against tests/golden; about 6 s
make pin-update               # rewrite the manifests after an intended change
make pin-images REF=<commit>  # render that commit's expected frames as PNGs
```

The goldens are text. Each frame is a SHA-256 hash of its width, height and raw RGBA pixels, so any one-pixel change fails. No images are committed.

On a mismatch, `make pin` exits nonzero and names each failing frame with its expected and actual hash. It writes the actual frames to `build/pin-out/<scenario>-<frame>-actual.png` and each scenario's log beside them. To see the expected frame, run `make pin-images REF=<last good commit>` (for example `HEAD`). It builds that commit in a worktree under `build/ref/` and saves every frame it displays as `build/pin-images/<short sha>/<hash prefix>.png`. Open the file named by the expected hash. `REF` must be a commit that already has the pin.

## How it works

`shim.h` is force-included ahead of every production file. It replaces four SFML pieces and `time()` with scripted fakes:

- `sf::RenderWindow` becomes an offscreen 1920x1080 render texture. Every `display()` is a captured frame.
- `sf::Clock` and `time()` read a fake clock that advances 1/8 s per frame.
- `sf::Mouse` and window events come from the scenario script.
- `sf::Music` is silent.

`pin.cpp` includes `src/main.cpp` with `main` renamed, so each scenario drives the real `main()` through menus, clicks and keys. Each scenario runs in its own process and sandbox directory, because the game reads and writes `pet_stats.txt` in its working directory.

## Goldens

- `tests/golden/<scenario>.txt` lists every displayed frame in order: index, window title, script step, and hash. A missing, extra or reordered frame fails.
- `tests/golden/<scenario>.save.txt` is the exact `pet_stats.txt` the game left behind.
- `tests/golden/logic.quotes.txt` is the quote picked at fixed clock values.

## What is pinned

- The start menu, including the "No Previous Games Found" message, and the pet picker.
- For each of the four pets: the home screen, every frame of the sleep, shower, medicine and eating animations, the shop (buying items and running out of coins), the fridge (eating a stocked and an unstocked item), the info screen, a 30 s stat decay, and the save file.
- Loading a save with all stats at 0, which shows the warnings.
- The math game (intro, rules, five answers, results) and the box minigame (intro, every 25th frame of play, game over).
- `logic`: `Stats` decay and caps; `readSave`; `findPet`; quote selection and `updateQuote`; minigame scoring by box height.
- `math_odd_answers`: a lone "-", a double "-" and a twelve-digit answer never crash the math game, and the score counts only real answers.
- `math_escape_intro`: Escape during the math intro returns to the pet.
- Two known bugs, pinned as current behavior: the quote never hides, and closing the start menu leaves the game spinning (`menu_close_spins`).

The unit tests (`make test`, `tests/unit/unit_tests.cpp`) build with the same `shim.h`, so they read the fake clock and open no window. Fixing one of these bugs updates its pin on purpose.

## What is not pinned

- Audio. The pin never plays any sound.
- Real-window behavior: OS scaling, a screen smaller than 1920x1080, frame pacing, and real mouse coordinates.
- Crashes that kill the process, such as loading an empty or truncated save.
- Portability. The goldens depend on macOS `rand()`, this SFML version, and its font rasterizer, so expect mismatches on other platforms. The hash uses macOS CommonCrypto.
