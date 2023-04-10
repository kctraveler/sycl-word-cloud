#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include "../utils/utils.hpp"
#include <chrono>

std::vector<size_t> hash_line(const std::string& str, const std::string& delimiters){
    std::vector<size_t> hashed_tokens = {};
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

std::vector<size_t> tokenize_file(std::string file_path){
    std::fstream words_file;
    std::vector<size_t> hashed_words = {};
    words_file.open(file_path);
    if (words_file.is_open()){
        std::string line;
        while(getline(words_file, line)){
            std::transform(line.begin(), line.end(), line.begin(),::toupper);
            std::vector<size_t> hashed_line = hash_line(line, SPECIAL_CHARACTERS);
            hashed_words.insert(hashed_words.end(), hashed_line.begin(), hashed_line.end());
        }
    }
    words_file.close();
    return hashed_words;
}

std::vector<short> count_words(std::vector<size_t> hashed_words){
    std::vector<short> counts(WORD_ID_RANGE, 0);
    for(size_t hash : hashed_words){
        counts[hash] += 1;
    }
    return counts;
}


int main(){
    auto start = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file("./data/hamlet.txt");
    auto step1 = std::chrono::high_resolution_clock::now();
    std::sort(hashed_words.begin(), hashed_words.end());
    auto sort_end = std::chrono::high_resolution_clock::now();
    auto counts = count_words(hashed_words);
    auto step2 = std::chrono::high_resolution_clock::now();
    //write_results("./data/parallel-ham-results.txt", counts);
    auto end = std::chrono::high_resolution_clock::now();

    // Calcuate and report back durations
    std::chrono::duration<float> total_duration = end - start;
    std::chrono::duration<float> step1_duration = step1 - start;
    std::chrono::duration<float> step2_duration = step2 - step1;
    std::chrono::duration<float> sort_duration = sort_end - step1;
    printf("Total Duration:\t%fs\n", total_duration.count());
    printf("Tokenize File Duration:\t%fs\n", step1_duration.count());
    printf("Count Words Duration:\t%fs\n", step2_duration.count());
    printf("Sort Tokens Duration:\t%f s\n", sort_duration.count());
}