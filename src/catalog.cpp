#include "catalog.h"

namespace {

const std::vector<Food> kAdultAvoFoods = {
    {"Steak", 500, "Images/Steak.png"},        {"Fried Rice", 200, "Images/FriedRice.png"},
    {"Curry Chicken", 200, "Images/Curry.png"}, {"Boba", 150, "Images/Boba.png"},
    {"Cold Rolls", 180, "Images/ColdRolls.png"}, {"Medicine", 500, "Images/Medicine.png"}};
const std::vector<Food> kBabyAvoFoods = {{"Baby Pumpkin", 100, "Images/Pumpkin.png"},
                                         {"Baby Banana", 50, "Images/Banana.png"},
                                         {"Yakult", 50, "Images/Yakult.png"},
                                         {"Baby Medicine", 500, "Images/BabyMedicine.png"}};
const std::vector<Food> kAdultGhostFoods = {
    {"Chicken", 100, "Images/Chicken.png"},    {"Milk", 50, "Images/Milk.png"},
    {"Apple", 50, "Images/Apple.png"},         {"Medicine", 500, "Images/Medicine.png"},
    {"Pizza", 70, "Images/Pizza.png"},         {"Fish", 70, "Images/Fish.png"}};
const std::vector<Food> kBabyGhostFoods = {{"Baby Milk", 50, "Images/Milk2.png"},
                                           {"Baby Porridge", 30, "Images/Porridge.png"},
                                           {"Yoghurt", 70, "Images/Yoghurt.png"},
                                           {"Baby Medicine", 500, "Images/BabyMedicine.png"}};

// Grid origins are the original windows' centering arithmetic, evaluated.
const ShopLayout kAdultShop = {"Adult Avo Shopping", {323, 128, 350, 400, 3}, 34, "NOT ENOUGH COINS"};

// Baby sheets are the same size as the adult ones; what differs is the file
// and, for the avos, how the frames are laid out.
const PetSpec kPets[] = {
    {"adult_avo", Species::Avo, Stage::Adult, "bigCharNormalAvo.png",
     {{"Big2/sleeping/spritesheet.png", 32, 32, 4, 3, 0.8f},
      {"Big2/showering/spritesheet.png", 32, 32, 5, 5, 1.4f},
      // The sheet is 32 px tall; the game has always sampled 36 and the pin
      // keeps that stretch.
      {"New Piskel-1 10.15.48ΓÇ»PM.png (2).png", 32, 36, 2, 2, 1.4f},
      {"Big2/eating/spritesheet.png", 32, 32, 3, 3, 1.4f}},
     kAdultAvoFoods,
     kAdultShop},
    {"baby_avo", Species::Avo, Stage::Baby, "smallAvoNormal.png",
     {{"Small2/Sleeping/spritesheet.png", 32, 32, 3, 3, 0.8f},
      {"Small2/Showering/spritesheet.png", 32, 32, 5, 5, 0.8f},
      {"Small2/medications/spritesheet.png", 32, 32, 2, 2, 1.4f},
      {"Small2/Eating/spritesheet.png", 32, 32, 3, 3, 1.4f}},
     kBabyAvoFoods,
     {"Baby Avo Shop", {173, 300, 300, 0, 4}, 24, "NOT ENOUGH GOLD"}},
    {"adult_ghost", Species::Ghost, Stage::Adult, "bigCharNormalGhost.png",
     {{"Big/Sleeping.png", 36, 36, 2, 3, 0.8f},
      {"Big/Shower.png", 36, 36, 2, 4, 0.8f},
      {"Big/Medicine .png", 32, 32, 1, 2, 1.4f},
      {"Big/eating.png", 32, 32, 2, 3, 1.4f}},
     kAdultGhostFoods,
     {"Shopping", kAdultShop.grid, 34, "NOT ENOUGH COINS"}},
    {"baby_ghost", Species::Ghost, Stage::Baby, "smallGhostNormal.png",
     {{"Small/Sleeping.png", 36, 36, 2, 3, 0.8f},
      {"Small/Shower.png", 36, 36, 2, 4, 0.8f},
      {"Small/Medicine .png", 32, 32, 1, 2, 1.4f},
      {"Small/eating.png", 32, 32, 2, 3, 1.4f}},
     kBabyGhostFoods,
     {"Baby Ghost Shopping", {560, 240, 250, 250, 2}, 24, "NOT ENOUGH COINS"}},
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
