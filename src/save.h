// pet_stats.txt, the one file the game reads and writes.
#pragma once

#include <map>
#include <optional>
#include <string>

#include "stats.h"

// "<pet> <health> <sleep> <hunger> <iq> <money>", a blank line, then one
// "<food name> <count>" per line.
struct SaveFile {
  std::string pet;
  Stats stats;
  std::map<std::string, int> pantry;
};

// Empty when the file is missing or its first line is not a full stats line.
std::optional<SaveFile> readSave(const std::string& path);

void writeSave(const std::string& path, const std::string& pet, const Stats& stats,
               const std::map<std::string, int>& pantry);
