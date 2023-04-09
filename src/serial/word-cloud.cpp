#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include "../utils/utils.hpp"


// TODO Decoder that takes a hash and returns the word.
// TODO Find the top N words in the count array
// TODO Refactor


std::map<size_t, short>  get_word_id_counts(std::vector<short> counts){
  std::map<size_t, short> hash_counts;
  for(size_t i = 0; i <  counts.size(); i++){
    if(counts[i] > 0){
      hash_counts[i] = counts[i];
    }
  }
  return hash_counts;
}

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



void write_results(std::string file_path, std::vector<short> counts){
  std::map<size_t, short> hash_counts = get_word_id_counts(counts);
  std::ofstream output_file;
  output_file.open(file_path);
  std::map<size_t, short>::iterator it = hash_counts.begin();
  while (it != hash_counts.end())
  {
    output_file << "Word ID: " << it->first << ", Count: " << it->second << std::endl;
    ++it;
  }
  output_file.close();
}



int main(){

  std::vector<short> words =read_file("./data/hamlet.txt");
  write_results("./data/processed_hamlet.txt", words);
  
}






