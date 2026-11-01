# Shop and eat

The shop window lists the pet's foods with prices. A click buys one if the coins cover it, or shows a no-coins message. The fridge window shows the pantry. Eating a food in stock fills hunger and plays the eating animation when the fridge closes. A food out of stock shows "NEED TO BUY MORE" and no animation plays.

## Sub-features

- `buy` takes coins and adds to the pantry and the trolley count.
- `no-coins` shows the pet's no-coins text.
- `eat` takes one from the pantry and sets hunger to 100.
- `out-of-stock` shows "NEED TO BUY MORE".
- `eat-animation` plays only if the pet ate during the visit.

## How to get to it (user POV)

- Click the cart button (`kShop`) or the cutlery button (`kEat`) on the home screen.

## Driving it with the pin

Preconditions: a game whose pet is known, so the window titles and item positions are known. Titles and grids come from `src/catalog.cpp`, for example `Adult Avo Shopping` and `Eating Adult Avo`.

- **Buy.** `s.click("Adult Avo Shopping", 398, 203, "shop: buy steak")`, then `s.close("Adult Avo Shopping", "shop: close")`.
- **Eat.** `s.click(kMain, kEat.x, kEat.y, "eat")`, `s.click("Eating Adult Avo", 760, 340, "eat: steak")`, `s.key("Eating Adult Avo", sf::Keyboard::Escape, "eat: close")`.
- **Proof.** `journey_*` buys until out of coins and eats a stocked then an unstocked item. `load_game` tries only an unstocked item and clicks Info two frames later, which proves no animation played.

## Gotchas

- Item centres differ per pet. `journey()` in `pin.cpp` lists them for all four.
- The save lists the pantry. Check it with `rec.checkText("save.txt", ...)`.
