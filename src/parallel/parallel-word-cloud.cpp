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



std::vector<short> count_words(std::vector<int> hashed_words){
    std::vector<short> counts(WORD_ID_RANGE, 0);
    for(int hash : hashed_words){
        counts[hash] += 1;
    }
    return counts;
}


int main(){

    // TODO CLI these parameters with default values below
    std::string input_path = "./data/hamlet.txt";
    std::string par_out_path = "./data/parallel-hamlet-results.txt";
    std::string seq_out_path = "./data/serial-hamlet-results.txt";

    // Shared Execution Steps
    auto start = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file(input_path);
    auto tokenize = std::chrono::high_resolution_clock::now();
    //std::sort(hashed_words.begin(), hashed_words.end());
    auto sort_end = std::chrono::high_resolution_clock::now();

    // Serial Execution Steps
    auto counts = count_words(hashed_words);
    auto serial_count = std::chrono::high_resolution_clock::now();
    write_results(seq_out_path, counts);
    auto write_results = std::chrono::high_resolution_clock::now();

    // Parallel Execution Steps
    // Parallel count
    auto start_parallel = std::chrono::high_resolution_clock::now();
    queue q;
    auto N = hashed_words.size();
    buffer<int> word_hash_buff{hashed_words};
    auto counts_vector = std::vector<short>(WORD_ID_RANGE);
    buffer<short> counts_buff{counts_vector};
    auto r = range<1>(N);
    //auto *data = malloc_shared<int>(N, q);
    //short *counts_malloc = malloc_shared<short>(WORD_ID_RANGE, q);
    //std::copy_n(hashed_words.begin(), N, data);
    auto nKernels = q.get_device().get_info<cl::sycl::info::device::max_compute_units>();
    std::string device_name = q.get_device().get_info<sycl::info::device::name>();
    // q.parallel_for(range<1>(N), [=](id<1> i) {counts_malloc[data[i]] += 1;}).wait();
    q.submit([&](handler& h){
        accessor in{word_hash_buff, h, read_only};
        accessor out{counts_buff, h, write_only, no_init};
        h.parallel_for(r, [=](id<1> i) {
            out[in[i]] += 1;
        });
    });

    auto end_parallel = std::chrono::high_resolution_clock::now();
    
    // Create map and write results to file.
    auto start_second_write = std::chrono::high_resolution_clock::now();
    host_accessor counts_acc{counts_buff, read_only};
    std::map<int, short> hash_counts;
    for(int i = 0; i < WORD_ID_RANGE; i++){
        if (counts_acc[i] != 0){
            hash_counts[i] = counts_acc[i];
        }
    }
    
    std::ofstream output_file;
    output_file.open(par_out_path);
    std::map<int, short>::iterator it = hash_counts.begin();
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