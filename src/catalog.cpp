#include "catalog.h"

#include <cstdlib>

namespace {

const std::vector<Food> kAdultAvoFoods = {
    {"Steak", 500, "assets/food/steak.png"},        {"Fried Rice", 200, "assets/food/fried-rice.png"},
    {"Curry Chicken", 200, "assets/food/curry.png"}, {"Boba", 150, "assets/food/boba.png"},
    {"Cold Rolls", 180, "assets/food/cold-rolls.png"}, {"Medicine", 500, "assets/food/medicine.png"}};
const std::vector<Food> kBabyAvoFoods = {{"Baby Pumpkin", 100, "assets/food/pumpkin.png"},
                                         {"Baby Banana", 50, "assets/food/banana.png"},
                                         {"Yakult", 50, "assets/food/yakult.png"},
                                         {"Baby Medicine", 500, "assets/food/medicine.png"}};
const std::vector<Food> kAdultGhostFoods = {
    {"Chicken", 100, "assets/food/chicken.png"},    {"Milk", 50, "assets/food/milk.png"},
    {"Apple", 50, "assets/food/apple.png"},         {"Medicine", 500, "assets/food/medicine.png"},
    {"Pizza", 70, "assets/food/pizza.png"},         {"Fish", 70, "assets/food/fish.png"}};
const std::vector<Food> kBabyGhostFoods = {{"Baby Milk", 50, "assets/food/milk.png"},
                                           {"Baby Porridge", 30, "assets/food/porridge.png"},
                                           {"Yoghurt", 70, "assets/food/yoghurt.png"},
                                           {"Baby Medicine", 500, "assets/food/medicine.png"}};

// Grid origins are the original windows' centering arithmetic, evaluated.
const ShopLayout kAdultShop = {"Adult Avo Shopping", {323, 128, 350, 400, 3}, 34, "NOT ENOUGH COINS"};
const Grid kAdultFridgeGrid = {685, 265, 200, 200, 3};

const std::vector<std::string> kAvoQuotes = {
    "What did the avocado say to the toast? You’re my butter half!",
    "If life gives you avocados, make guacamole!",
    "I’m just an avocado, living in a toast world.",
    "I’m not a regular avocado, I’m a cool avocado.",
    "You guac my world!",
    "An avocado a day keeps the bad vibes away.",
    "How does an avocado propose? With a guac ring!",
    "Just avocado-ing with the flow!"};

const std::vector<std::string> kGhostQuotes = {
    "I don’t mean to scare you, but I am just here for the boos!",
    "Ghosts are just like us—constantly looking for a good haunt!",
    "If you think I'm scary now, you should see me before my coffee!",
    "What do you call a ghost that tells jokes? A pun-derworld spirit!",
    "Boo! Just kidding. I’m just here for the snacks.",
    "Why dont ghosts like rain? Because it dampens their spirits!"};

const PetSpec kPets[] = {
    {"adult_avo", Species::Avo, Stage::Adult, "assets/pets/adult_avo/idle.png",
     {{"assets/pets/adult_avo/sleep.png", 32, 32, 4, 3, 0.8f},
      {"assets/pets/adult_avo/shower.png", 32, 32, 5, 5, 1.4f},
      {"assets/pets/adult_avo/medicine.png", 32, 32, 2, 2, 1.4f},
      {"assets/pets/adult_avo/eat.png", 32, 32, 3, 3, 1.4f}},
     kAdultAvoFoods,
     kAdultShop,
     {"Eating Adult Avo", kAdultFridgeGrid, 15}},
    {"baby_avo", Species::Avo, Stage::Baby, "assets/pets/baby_avo/idle.png",
     {{"assets/pets/baby_avo/sleep.png", 32, 32, 3, 3, 0.8f},
      {"assets/pets/baby_avo/shower.png", 32, 32, 5, 5, 0.8f},
      {"assets/pets/baby_avo/medicine.png", 32, 32, 2, 2, 1.4f},
      {"assets/pets/baby_avo/eat.png", 32, 32, 3, 3, 1.4f}},
     kBabyAvoFoods,
     {"Baby Avo Shop", {173, 300, 300, 0, 4}, 24, "NOT ENOUGH GOLD"},
     {"Eating Baby Avo", {510, 150, 450, 450, 2}, 15}},
    {"adult_ghost", Species::Ghost, Stage::Adult, "assets/pets/adult_ghost/idle.png",
     {{"assets/pets/adult_ghost/sleep.png", 36, 36, 2, 3, 0.8f},
      {"assets/pets/adult_ghost/shower.png", 36, 36, 2, 4, 0.8f},
      {"assets/pets/adult_ghost/medicine.png", 32, 32, 1, 2, 1.4f},
      {"assets/pets/adult_ghost/eat.png", 32, 32, 2, 3, 1.4f}},
     kAdultGhostFoods,
     {"Shopping", kAdultShop.grid, 34, "NOT ENOUGH COINS"},
     {"Eating Ghost Window", kAdultFridgeGrid, 5}},
    {"baby_ghost", Species::Ghost, Stage::Baby, "assets/pets/baby_ghost/idle.png",
     {{"assets/pets/baby_ghost/sleep.png", 36, 36, 2, 3, 0.8f},
      {"assets/pets/baby_ghost/shower.png", 36, 36, 2, 4, 0.8f},
      {"assets/pets/baby_ghost/medicine.png", 32, 32, 1, 2, 1.4f},
      {"assets/pets/baby_ghost/eat.png", 32, 32, 2, 3, 1.4f}},
     kBabyGhostFoods,
     {"Baby Ghost Shopping", {560, 240, 250, 250, 2}, 24, "NOT ENOUGH COINS"},
     {"Eating Baby Ghost", {585, 265, 200, 200, 2}, 15}},
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
  std::abort();  // kPets has every species and stage
}

const std::vector<std::string>& quotesFor(Species species) {
  return species == Species::Avo ? kAvoQuotes : kGhostQuotes;
}

const char* soundPath(Species species) {
  return species == Species::Avo ? "assets/audio/avo.mp3"
                                 : "assets/audio/ghost.mp3";
}
