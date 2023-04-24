// Objective of this version is to make the windowed approach more task based or
// with custom defined partitioners
#include "../utils/utils.hpp"
#include <CL/sycl.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

const int WINDOW_SIZE = 10000;

std::vector<std::map<int, short>> count_words(std::vector<int> hashed_words){
    std::vector<std::map<int, short>> seq_windows{};
    for (int i = 0; i <= hashed_words.size() / WINDOW_SIZE; i += 1){
        std::vector<short> window_counts(WORD_ID_RANGE, 0);
        for (int j = i * WINDOW_SIZE; j < i + WINDOW_SIZE; j++){
            window_counts[hashed_words[j]] += 1;
        }
        auto count_map = get_word_id_counts(window_counts);
        std::cout << count_map[hashed_words[0]] << std::endl;
        seq_windows.push_back(count_map);
    }
   return seq_windows;
}

int main(){
    std::string input_path = "./data/hamlet.txt";
    std::string par_out_path = "./data/parallel-hamlet-results.txt";
    std::string seq_out_path = "./data/serial-hamlet-results.txt";

    auto start = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file(input_path);
    auto tokenize = std::chrono::high_resolution_clock::now();

    // Serial Execution Steps
    auto counts = count_words(hashed_words);
    std::cout << "Other Stats" << std::endl;
    std::cout << hashed_words.size() << std::endl;
    std::cout << counts.size() << std::endl;
    auto serial_count = std::chrono::high_resolution_clock::now();
    
   
}