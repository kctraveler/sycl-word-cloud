#pragma once
#include <string>
#include <map>
#include<vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// Simple Header library to share code between the serial and parallell versions


// Constants
const std::string SPECIAL_CHARACTERS = " @#$%^&*[]!.?,;-";
const size_t WORD_ID_RANGE = 1740000000;

extern std::size_t hash(std::string word, size_t range);
extern std::map<int, short> get_word_id_counts(std::vector<short> counts);
extern void write_results(std::string file_path, std::vector<short> counts);
extern std::vector<int> hash_line(const std::string& str, const std::string& delimiters);
extern std::vector<int> tokenize_file(std::string file_path);