// pet_stats.txt, the one file the game reads and writes.
#pragma once

#include <map>
#include <optional>
#include <string>

// "<pet> <health> <sleep> <hunger> <iq> <money>", a blank line, then one
// "<food name> <count>" per line.
struct SaveFile {
  std::string pet;
  int health = 0, sleep = 0, hunger = 0, iq = 0, money = 0;
  std::map<std::string, int> pantry;
};

// Empty when the file is missing or its first line is not a full stats line.
std::optional<SaveFile> readSave(const std::string& path);
