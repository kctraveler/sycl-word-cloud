#include "utils.hpp"
#include <string>

std::size_t hash(std::string word, size_t range){
  size_t hash = std::hash<std::string>{}(word);
  return hash % range;
}