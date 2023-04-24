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
        int start = i * WINDOW_SIZE;
        int end = start + WINDOW_SIZE;
        for (int j = start; j < end && j < hashed_words.size(); j++){
            window_counts[hashed_words[j]] += 1;
        }
        auto count_map = get_word_id_counts(window_counts);
        seq_windows.push_back(count_map);
    }
   return seq_windows;
}

int main(){
    std::string input_path = "./data/hamlet.txt";
    std::string par_out_path = "./data/parallel-hamlet-results.txt";
    std::string seq_out_path = "./data/serial-hamlet-results.txt";

    //Tokenize File with word IDs
    auto start_tokenize = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file(input_path);
    auto end_tokenize = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> tokenize_duration = end_tokenize - start_tokenize;
    printf("Tokenize File Duration:\t\t\t%f s\n", tokenize_duration.count());

    // Windowed Serial Execution Steps
    auto start_serial = std::chrono::high_resolution_clock::now();
    auto counts = count_words(hashed_words);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> serial_count_duration = end_serial - start_serial;
    printf("Count Words Duration:\t\t\t%f s\n", serial_count_duration.count());
}