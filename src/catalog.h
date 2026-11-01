// Everything that differs between the four pets, as data. Code elsewhere takes
// a PetSpec and never asks which pet it is.
#pragma once

#include <string>
#include <vector>

enum class Species { Avo, Ghost };
enum class Stage { Adult, Baby };

// Also indexes PetSpec::sheets.
enum class Activity { Sleep = 0, Shower, Medicine, Eat };

// One activity's sprite sheet: frame i is the cell at (i % cols, i / cols).
struct Sheet {
  const char* path;
  int cellW, cellH;
  int cols;
  int frames;
  float frameSeconds;
};

struct Food {
  const char* name;
  int price;
  const char* icon;
};

// Cell i of a grid of 150 px icons sits at (x + (i % cols) * dx, y + (i / cols) * dy).
struct Grid {
  int x, y;
  int dx, dy;
  int cols;
};

struct ShopLayout {
  const char* title;
  Grid grid;
  unsigned labelSize;
  const char* noCoinsText;
};

struct FridgeLayout {
  const char* title;
  Grid grid;
  float labelGap;  // between an icon and its "Quantity" line
};

struct PetSpec {
  const char* name;  // the save-file token, e.g. "adult_avo"
  Species species;
  Stage stage;
  const char* idleSprite;
  Sheet sheets[4];
  std::vector<Food> foods;
  ShopLayout shop;
  FridgeLayout fridge;
};

const PetSpec* findPet(const std::string& name);
const PetSpec& findPet(Species, Stage);
const char* soundPath(Species);
const std::vector<std::string>& quotesFor(Species);
