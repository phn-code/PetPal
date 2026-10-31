// Everything that differs between the four pets, as data. Code elsewhere takes
// a PetSpec and never asks which pet it is.
#pragma once

#include <string>

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

struct PetSpec {
  const char* name;  // the save-file token, e.g. "adult_avo"
  Species species;
  Stage stage;
  const char* idleSprite;
  Sheet sheets[4];
};

const PetSpec* findPet(const std::string& name);
const PetSpec& findPet(Species, Stage);
