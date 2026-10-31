#include "catalog.h"

namespace {

// Baby sheets are the same size as the adult ones; what differs is the file
// and, for the avos, how the frames are laid out.
const PetSpec kPets[] = {
    {"adult_avo", Species::Avo, Stage::Adult, "bigCharNormalAvo.png",
     {{"Big2/sleeping/spritesheet.png", 32, 32, 4, 3, 0.8f},
      {"Big2/showering/spritesheet.png", 32, 32, 5, 5, 1.4f},
      // The sheet is 32 px tall; the game has always sampled 36 and the pin
      // keeps that stretch.
      {"New Piskel-1 10.15.48ΓÇ»PM.png (2).png", 32, 36, 2, 2, 1.4f},
      {"Big2/eating/spritesheet.png", 32, 32, 3, 3, 1.4f}}},
    {"baby_avo", Species::Avo, Stage::Baby, "smallAvoNormal.png",
     {{"Small2/Sleeping/spritesheet.png", 32, 32, 3, 3, 0.8f},
      {"Small2/Showering/spritesheet.png", 32, 32, 5, 5, 0.8f},
      {"Small2/medications/spritesheet.png", 32, 32, 2, 2, 1.4f},
      {"Small2/Eating/spritesheet.png", 32, 32, 3, 3, 1.4f}}},
    {"adult_ghost", Species::Ghost, Stage::Adult, "bigCharNormalGhost.png",
     {{"Big/Sleeping.png", 36, 36, 2, 3, 0.8f},
      {"Big/Shower.png", 36, 36, 2, 4, 0.8f},
      {"Big/Medicine .png", 32, 32, 1, 2, 1.4f},
      {"Big/eating.png", 32, 32, 2, 3, 1.4f}}},
    {"baby_ghost", Species::Ghost, Stage::Baby, "smallGhostNormal.png",
     {{"Small/Sleeping.png", 36, 36, 2, 3, 0.8f},
      {"Small/Shower.png", 36, 36, 2, 4, 0.8f},
      {"Small/Medicine .png", 32, 32, 1, 2, 1.4f},
      {"Small/eating.png", 32, 32, 2, 3, 1.4f}}},
};

}  // namespace

const PetSpec* findPet(const std::string& name) {
  for (const PetSpec& pet : kPets)
    if (name == pet.name) return &pet;
  return nullptr;
}

const PetSpec& findPet(Species species, Stage stage) {
  for (const PetSpec& pet : kPets)
    if (pet.species == species && pet.stage == stage) return pet;
  return kPets[0];
}
