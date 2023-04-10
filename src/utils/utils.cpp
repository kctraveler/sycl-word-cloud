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
    if(counts[i] > 0){
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