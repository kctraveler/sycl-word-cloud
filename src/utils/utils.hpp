#pragma once
#include <string>

// Simple Header library to share code between the serial and parallell versions


// Constants
const std::string SPECIAL_CHARACTERS = " @#$%^&*[]!.?,;-";
const size_t WORD_ID_RANGE = 1740000000;

extern std::size_t hash(std::string word, size_t range);