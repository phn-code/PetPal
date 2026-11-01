---
name: verify
description: Prove a change to the PetPal SFML game the way a player sees it. Drives the real main() through scripted clicks and keys with the offscreen pin harness, saves every frame as a PNG to look at, and smoke-launches the real window. Use after any change to src/ that affects what the game draws or how it reacts to input, and to reproduce a UI bug before fixing it.
---

# Verify PetPal

PetPal is a desktop game: one SFML window per scene, 1920x1080 scene coordinates, assets loaded by relative path. Two ways to drive it exist.

- The pin (`tests/pin/`) runs the real `main()` offscreen against scripted input and a fake clock. Every displayed frame is hashed against `tests/golden`. Use it for every proof. It is deterministic and needs no screen.
- The real window. Agents cannot click in it, because macOS input needs Accessibility permission. Use it only as a smoke launch and a screenshot.

Read `features/README.md` for the per-feature recipes.

## Launch

All commands run from the repository root.

```sh
make check                                      # build (-Werror), unit tests, pin, check-assets, lint
.claude/skills/verify/scripts/smoke.sh 4 --screenshot
```

`smoke.sh` builds, starts `build/Petpal` in `build/verify/run/` with an `assets` symlink, waits the given seconds, and stops the process by its PID. It prints `smoke ok` when the game stayed up with no load errors. The game writes `pet_stats.txt` to its working directory, so the sandbox keeps the player's real save safe. Ready means the start menu is on screen: open `build/verify/screen.png` to see it.

## Doctor

Run these when anything looks off. All are read-only except the build.

- `brew --prefix sfml@2` prints a path. If it does not, run `brew install sfml@2`.
- `make -q all; echo $?` prints `0` when `build/Petpal` is newer than every source.
- `make check-assets` ends with `0 problems`.
- `pgrep -fl build/Petpal` lists a game you or the user started. Never stop one you did not start.

## Drive

To see the frames of an existing scenario, run:

```sh
.claude/skills/verify/scripts/frames.sh <scenario>
```

It writes `build/verify/frames/<scenario>/index.tsv`, one row per frame: index, window title, script step, and PNG path. Read the PNGs for the steps you care about. `build/pin --list` prints the scenario names. The script never touches `tests/golden`.

To prove new behavior, write a scenario in `tests/pin/pin.cpp`:

1. Write a function `int myCase(Recorder& rec)` that builds a `Script s(rec);`, queues input, calls `petpal_main()`, and checks the result. Copy the shape of `loadGame` or `smallScreen`.
2. Queue input with `s.click(window, x, y, label, wait)`, `s.key(...)`, `s.close(...)`, `s.advance(window, sf::seconds(n), label)` and `s.then(window, wait, label, fn)`. Each action fires after `wait` more frames of the window titled `window`. Coordinates are window pixels. On the default 1920x1080 screen they equal scene coordinates. The home buttons are the constants `kSleep`, `kShower`, `kGame`, `kEat`, `kMedicine`, `kShop`, `kMath`, `kInfo`, `kTouch`, and the menu ones are `kMenuNew` and `kMenuLoad`.
3. Assert what the player sees or keeps. `rec.check(cond, message)` checks a fact, `rec.checkText("save.txt", readFile("pet_stats.txt"))` pins the save, and `s.inspect` sees every frame as a `sf::PinWindow` (see `smallScreen`). A click that never lands shows up as `display budget exhausted during step "<label>"`, because the script waits forever for a window that never opens.
4. Add the name to `kScenarios` and to the dispatch in `main()` at the bottom of the file.
5. Before the fix, run `make pin` and keep the failure text. After it, run `make pin-update` and `git diff --stat tests/golden`. Only the goldens you expected may change. Look at changed frames with `frames.sh`.

For a scenario that has no frames worth pinning, set `s.capturing = false`, as `save_writes` does.

## Evidence

A proof is the pin output before and after the change, the frame PNGs of the steps that changed, and `git diff --stat tests/golden`. Quote the failing line, such as `PIN FAIL load_game: threw: display budget exhausted during step "info"`, not just "it failed". For anything the pin fakes, also run `smoke.sh` and look at `build/verify/screen.png`. The pin fakes audio, real window resizing, Retina scaling, and real mouse input.

Evidence lives under `build/verify/` and `build/pin-out/`. `make pin` empties `build/pin-out` on every run, so copy anything you need first. `build/` is ignored by git.

## Cleanup

`smoke.sh` stops the game it started. `frames.sh` and the pin leave no processes. To reclaim disk, delete `build/verify/run` and `build/pin-sandbox`. Keep `build/verify/*.png`, `smoke.log`, and `frames/` until the proof is reported. `make clean` deletes all of `build/`, evidence included.

## Gotchas

- Run every command from the repository root. The binaries load `assets/...` relative to the working directory.
- The goldens depend on macOS `rand()`, this SFML version, and its font rasterizer. Expect mismatches on other platforms.
- `make pin-images REF=<commit>` renders a committed version's frames in a worktree under `build/ref/`. Use it to see the expected frame named in a failure, and `frames.sh` for the working tree.
- A golden change needs its file named in the commit message, or the `commit-msg` hook rejects the commit.
