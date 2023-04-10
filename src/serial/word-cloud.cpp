#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include "../utils/utils.hpp"
#include <chrono>

// TODO Decoder that takes a hash and returns the word.
// TODO Find the top N words in the count array
// TODO Refactor




void count_words(const std::string& str, const std::string& delimiters,std::vector<short>& counts) {
    
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token)) {
        size_t start = 0, end = 0;
        while ((end = token.find_first_of(delimiters, start)) != std::string::npos) {
            if (end != start) {
                // tokens.push_back(token.substr(start, end - start));
              counts[hash(token.substr(start, end - start), WORD_ID_RANGE)] += 1;
            }
            start = end + 1;
        }
        if (start < token.size()) {
            //tokens.push_back(token.substr(start));
            counts[hash(token.substr(start, end - start), WORD_ID_RANGE)] += 1;
        }
    }
}

std::vector<short> read_file(std::string file_path){
  std::fstream words_file;
  std::vector<short> counts(WORD_ID_RANGE, 0);
  words_file.open(file_path);
  if (words_file.is_open()){
    std::string line;
    while (getline(words_file, line)) {
      std::transform(line.begin(), line.end(), line.begin(), ::toupper);
      count_words(line, SPECIAL_CHARACTERS, counts);
    }
  }
  words_file.close();
  return counts;
}




int main(){
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<short> words =read_file("./data/hamlet.txt");
  write_results("./data/original_processed_hamlet.txt", words);
   auto end = std::chrono::high_resolution_clock::now();

   std::chrono::duration<float> total_time = end - start;
   printf("\n\nTOTAL TIME\t\t\t\t%f s\n", total_time.count());

}






