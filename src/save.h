// pet_stats.txt, the one file the game reads and writes.
#pragma once

#include <map>
#include <optional>
#include <string>

#include "catalog.h"
#include "stats.h"

// Food name -> how many are in stock. Foods that ran out have no entry.
using Pantry = std::map<std::string, int>;

// "<pet> <health> <sleep> <hunger> <iq> <money>", a blank line, then one
// "<food name> <count>" per line.
struct SaveFile {
  const PetSpec* pet;
  Stats stats;
  Pantry pantry;
};

// Empty when the file is missing or is not a save of a known pet with a
// non-negative count on every food line.
std::optional<SaveFile> readSave(const std::string& path);

std::string formatSave(const PetSpec& pet, const Stats& stats, const Pantry& pantry);

// Writes a temporary file and renames it over `path`, so a crash mid-write
// leaves the previous save intact.
void writeSave(const std::string& path, const std::string& text);
