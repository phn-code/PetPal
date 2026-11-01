# Minigames

The math game (`Math Game` window) plays an intro, shows rules, asks five questions, and adds 5 IQ per right answer. The box game (`PetPal` window) moves a slider to catch falling boxes and pays 5 coins per point.

## Sub-features

- `math-play` answers five questions and shows the score.
- `math-escape` Escape during the intro returns to the pet.
- `boxes-play` starts with Enter and scores caught boxes.
- `boxes-escape` Escape closes the box game.

## How to get to it (user POV)

- Click the graph button (`kMath`) or the controller button (`kGame`) on the home screen.

## Driving it with the pin

Preconditions: a new game started with `startNewGame`.

- **Math answers.** Call `mathAnswers()` in the action that clicks `kMath`. It replays the game's `rand()` seed and returns the five right answers. Type one with `typeKeys(answer)`. See `games`.
- **Box game.** `.then("PetPal", 1, "minigame: start", ...)` pushes Enter. Set `s.sampleEvery = 25` while boxes fall to keep the golden small.
- **Proof.** `games`, `math_odd_answers`, and `math_escape_intro`. `logic` checks box scoring by height.

## Gotchas

- The math intro runs 3 s (24 frames) before the rules page takes Enter.
- The math game seeds `rand()` from the clock when it opens, so compute answers in the same action that opens it.
