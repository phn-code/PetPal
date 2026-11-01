#include "save.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

std::optional<SaveFile> readSave(const std::string& path) {
  std::ifstream in(path);
  std::string line;
  if (!std::getline(in, line)) return std::nullopt;
  SaveFile save;
  Stats& s = save.stats;
  std::string pet;
  std::istringstream head(line);
  if (!(head >> pet >> s.health >> s.sleep >> s.hunger >> s.iq >> s.money)) return std::nullopt;
  save.pet = findPet(pet);
  if (!save.pet) return std::nullopt;
  while (std::getline(in, line)) {
    // Food names contain spaces, so the count is the last word.
    std::istringstream words(line);
    std::string word, name;
    while (words >> word) name += (name.empty() ? "" : " ") + word;
    if (name.empty()) continue;
    std::size_t split = name.find_last_of(' ');
    if (split == std::string::npos) return std::nullopt;
    std::istringstream countText(name.substr(split + 1));
    int count;
    if (!(countText >> count) || !countText.eof() || count < 0) return std::nullopt;
    save.pantry[name.substr(0, split)] = count;
  }
  return save;
}

std::string formatSave(const PetSpec& pet, const Stats& s, const Pantry& pantry) {
  std::ostringstream out;
  out << pet.name << " " << s.health << " " << s.sleep << " " << s.hunger << " " << s.iq << " "
      << s.money << "\n\n";
  for (const auto& entry : pantry) out << entry.first << " " << entry.second << "\n";
  return out.str();
}

void writeSave(const std::string& path, const std::string& text) {
  const std::string temp = path + ".tmp";
  std::ofstream out(temp);
  out << text;
  out.close();
  if (!out) {
    std::cerr << "Could not write " << temp << "; the previous save is kept" << std::endl;
    std::remove(temp.c_str());
    return;
  }
  std::rename(temp.c_str(), path.c_str());
}
