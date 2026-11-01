# Start a game

The start menu offers New Game and Load Previous Game. New Game opens the pet picker (Avo or Ghost, then Adult or Baby). Load reads `pet_stats.txt` and goes straight to the pet.

## Sub-features

- `menu-new` starts the picker.
- `menu-load` loads a readable save, or shows "No Previous Games Found".
- `picker` picks one of four pets.
- `close-quits` closing the window on the menu or picker quits without a save.

## How to get to it (user POV)

- Launch the game. The menu is the first window, titled `My Virtual Pet`.

## Driving it with the pin

Preconditions: no `pet_stats.txt`, or one written by the scenario.

- **New game.** `startNewGame(s, kIntroAvo, kIntroAdult)` clicks Load (no save), New Game, the species, and the age. The home screen follows.
- **Load.** Write `pet_stats.txt` before `petpal_main()`, then `s.click(kMain, kMenuLoad.x, kMenuLoad.y, "menu: load")`. See `loadGame`.
- **Proof.** Scenarios `journey_*`, `load_game`, `bad_save`, and `close_quits`. Run `scripts/frames.sh journey_baby_ghost` and read the `intro:` frames.

## Gotchas

- An unreadable save counts as no save. `bad_save` pins that.
- The picker's last frame displays twice. The goldens expect it.
