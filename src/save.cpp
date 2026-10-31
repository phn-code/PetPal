#include "save.h"

#include <fstream>
#include <sstream>

std::optional<SaveFile> readSave(const std::string& path) {
  std::ifstream in(path);
  std::string line;
  if (!std::getline(in, line)) return std::nullopt;
  SaveFile save;
  std::istringstream head(line);
  if (!(head >> save.pet >> save.health >> save.sleep >> save.hunger >> save.iq >> save.money))
    return std::nullopt;
  while (std::getline(in, line)) {
    // Food names contain spaces, so the count is the last word.
    std::istringstream words(line);
    std::string word, name;
    while (words >> word) name += (name.empty() ? "" : " ") + word;
    if (name.empty()) continue;
    std::size_t split = name.find_last_of(' ');
    if (split == std::string::npos)
      save.pantry[name] = 0;
    else
      save.pantry[name.substr(0, split)] = std::stoi(name.substr(split + 1));
  }
  return save;
}
