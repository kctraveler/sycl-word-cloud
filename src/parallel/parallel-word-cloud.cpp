#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include "../utils/utils.hpp"
#include <chrono>
#include <sycl/sycl.hpp>
#include <CL/sycl.hpp>

using namespace sycl;

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
    // Shared Execution Steps
    auto start = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file("./data/hamlet.txt");
    auto tokenize = std::chrono::high_resolution_clock::now();
    std::sort(hashed_words.begin(), hashed_words.end());
    auto sort_end = std::chrono::high_resolution_clock::now();

    // Serial Execution Steps
    auto counts = count_words(hashed_words);
    auto serial_count = std::chrono::high_resolution_clock::now();
    write_results("./data/serial-hamlet-results.txt", counts);
    auto write_results = std::chrono::high_resolution_clock::now();

    // Parallel Execution Steps
    // Parallel count
    auto start_parallel = std::chrono::high_resolution_clock::now();
    queue q;
    auto N = hashed_words.size();
    size_t *data = malloc_shared<size_t>(N, q);
    short *counts_malloc = malloc_shared<short>(WORD_ID_RANGE, q);
    std::copy_n(hashed_words.begin(), N, data);
    auto nKernels = q.get_device().get_info<cl::sycl::info::device::max_compute_units>();
    std::string device_name = q.get_device().get_info<sycl::info::device::name>();
    q.parallel_for(range<1>(N), [=](id<1> i) {counts_malloc[data[i]] += 1;}).wait();
    auto end_parallel = std::chrono::high_resolution_clock::now();
    
    // Create map and write results to file.
    auto start_second_write = std::chrono::high_resolution_clock::now();
    std::map<size_t, short> hash_counts;
    for(size_t i = 0; i < WORD_ID_RANGE; i++){
        if (counts_malloc[i] != 0){
            hash_counts[i] = counts_malloc[i];
        }
    }
    std::string file_path = "./data/parallel-hamlet-results.txt";
    std::ofstream output_file;
    output_file.open(file_path);
    std::map<size_t, short>::iterator it = hash_counts.begin();
    while (it != hash_counts.end())
    {
        output_file << "Word ID: " << it->first << ", Count: " << it->second << std::endl;
        ++it;
    }
    output_file.close();
    auto end_second_write = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();


    // Calcuate and report back durations
    std::chrono::duration<float> tokenize_duration = tokenize - start;
    std::chrono::duration<float> serial_count_duration = serial_count - sort_end;
    std::chrono::duration<float> sort_duration = sort_end - tokenize;
    std::chrono::duration<float> parallel_duration = end_parallel - start_parallel;
    std::chrono::duration<float> write_file_duration = write_results - serial_count;
    std::chrono::duration<float> second_file_write = end_second_write - start_second_write;
    std::chrono::duration<float> total_time = end - start;
    printf("Tokenize File Duration:\t\t\t%f s\n", tokenize_duration.count());
    printf("Sort Tokens Duration:\t\t\t%f s\n", sort_duration.count());
    printf("Count Words Duration:\t\t\t%f s\n", serial_count_duration.count());
    printf("Time to write to file (single file):\t%f s\n", write_file_duration.count());
    printf("\nParallel Results on %s with %u processesors.\n", device_name.c_str(), nKernels);
    printf("Parallel Count Duration:\t\t%f s\n", parallel_duration.count());
    printf("Time to write parallel results:\t\t%f s\n", second_file_write.count());
    printf("\n\nTOTAL TIME\t\t\t\t%f s\n", total_time.count());
}