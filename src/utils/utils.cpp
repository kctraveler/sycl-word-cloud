#include "utils.hpp"
#include <string>
#include <map>
#include<vector>
#include <fstream>

std::size_t hash(std::string word, size_t range){
  size_t hash = std::hash<std::string>{}(word);
  return hash % range;
}

std::map<size_t, short>  get_word_id_counts(std::vector<short> counts){
  std::map<size_t, short> hash_counts;
  for(size_t i = 0; i <  counts.size(); i++){
    if(counts[i] != 0){
      hash_counts[i] = counts[i];
    }
  }
  return hash_counts;
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

std::vector<int> hash_line(const std::string& str, const std::string& delimiters){
    std::vector<int> hashed_tokens = {};
    std::string token;
    std::stringstream ss(str);
    while(std::getline(ss, token)){
        size_t start = 0, end = 0;
        while ((end = token.find_first_of(delimiters, start)) != std::string::npos){
            if (end != start) {
                hashed_tokens.push_back(hash(token.substr(start, end - start), WORD_ID_RANGE));
            }
            start = end + 1;
        }
        if(start < token.size()) {
            hashed_tokens.push_back(hash(token.substr(start, end - start), WORD_ID_RANGE));
        }
    }
    return hashed_tokens;
}

std::vector<int> tokenize_file(std::string file_path){
    std::fstream words_file;
    std::vector<int> hashed_words = {};
    words_file.open(file_path);
    if (words_file.is_open()){
        std::string line;
        while(getline(words_file, line)){
            std::transform(line.begin(), line.end(), line.begin(),::toupper);
            std::vector<int> hashed_line = hash_line(line, SPECIAL_CHARACTERS);
            hashed_words.insert(hashed_words.end(), hashed_line.begin(), hashed_line.end());
        }
    }
    words_file.close();
    return hashed_words;
}