# PetPal for agents

A C++17 SFML 2 game. `README.md` has the layout. Run every command from the repository root, because the game loads `assets/` by relative path.

## Commands

- `make check` runs the build, unit tests, pin, asset check, and lint, and installs the commit hook. It must pass before every commit.
- `make` builds `build/Petpal`. `make test` runs the unit tests. `make pin` compares every frame of scripted play with `tests/golden`.
- `make pin-update` rewrites the goldens. `make pin-images REF=<commit>` renders a commit's expected frames.
- To prove a UI change or reproduce a UI bug, use the `verify` skill in `.claude/skills/verify/`.

## Rules

Each rule names what enforces it.

- Warnings fail the build. `-Werror` in the `Makefile` enforces it.
- The pin stays green. To fix a bug, first add or change a pin scenario (or a unit test) that fails, and keep its failure text for the commit message. `make pin` enforces it.
- Goldens change only for an intended change. Run `make pin-update`, check that `git diff --stat tests/golden` lists only the files you expected, and look at the changed frames. Name each changed golden file in the commit body and say why it changed. `.githooks/commit-msg` rejects a commit that changes a golden it does not name. Saying why is on you.
- Anything that differs between pets goes in the pet's `PetSpec` entry in `src/catalog.cpp`, or a `Species` lookup next to `soundPath()`. Never write `if (pet.species == ...)` elsewhere. `make lint` enforces it.
- Every frame starts with `clearScene(window, colour)` and every click goes through `toScene()`, both from `src/scene.h`. `make lint` enforces it.
- Assets live under `assets/` and are named by a literal path in the code. Delete an asset when the last reference goes. `make check-assets` enforces it.
- Commit messages follow the recent log: `fix:`, `refactor:`, `build:`, `test:`, `docs:`, or `chore:`, then a body that says what the player sees change. No check enforces this one.
