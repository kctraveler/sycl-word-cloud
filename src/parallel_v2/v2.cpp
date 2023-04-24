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
#include <CL/sycl.hpp>

const int WINDOW_SIZE = 10000;

std::vector<std::map<int, short>> seq_count_words(std::vector<int> hashed_words){
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

void sub_buffer_count_words(std::vector<int> int_hashed_words){
    // Got the error message below when trying to use vector of int after a few iterations. 
    // Commented lines related to the error that can be swapped to go back to int for future investigation.
    // Specified offset of the sub-buffer being constructed is not a multiple of the memory base address alignment
    std::vector<size_t> hashed_words(int_hashed_words.begin(), int_hashed_words.end());
    //std::vector<int> hashed_words{int_hashed_words.begin(), int_hashed_words.end() }; // base address alignment error
    sycl::queue q;
    //sycl::buffer<int> b_hash_words{hashed_words};// base address alignment error
    sycl::buffer<size_t> b_hash_words{hashed_words}; 
    int N = hashed_words.size();
    int num_sub_buffs = N / WINDOW_SIZE;
    for (int i = 0; i <= num_sub_buffs; i++){
        size_t start = i * WINDOW_SIZE;
        size_t end = WINDOW_SIZE;
        std::cout << "BufferSize  " << b_hash_words.size() << std::endl;
        std::cout << "Start  " << start << std::endl;
        if (end + start >= N) {
            end = N - start;
        }
        std::cout << "End " << sycl::range{end}.size() << std::endl;
        //sycl::buffer<int> sub_buff(b_hash_words, start, sycl::range{end}); 
        sycl::buffer<size_t> sub_buff(b_hash_words, start, sycl::range{end}); //base address alignment error
        q.submit([&](sycl::handler& h){
            sycl::accessor in{sub_buff, h, sycl::read_only};
            h.parallel_for(WINDOW_SIZE, [=](sycl::id<1> i) {
                int result = in[i] + 1;
            });
        }).wait();
    }
}

int main(int argc, char* argv[]) {
    CLI::App app{"Parallel Word Cloud V2 OneAPI"};

    // TODO CLI these parameters with default values below
    std::string input_path = "./data/hamlet.txt";
    app.add_option("-i,--input", input_path, "Input Path");

    std::string par_out_path = "./data/parallel-hamlet-results.txt";
            app.add_option("-p,--parallel", par_out_path, "Parallel Output Path");

    std::string seq_out_path = "./data/serial-hamlet-results.txt";
        app.add_option("-s,--seq", seq_out_path, "Seq Output Path");

    int WINDOW_SIZE = 10000;
        app.add_option("-w,--window", WINDOW_SIZE, "Window Size");

    // Parse the command-line arguments
    CLI11_PARSE(app, argc, argv);
    // Print the parsed arguments
        fmt::print("Parsed arguments:\n");
        fmt::print("  input: {}\n", input_path);
        fmt::print("  parallel: {}\n", par_out_path);
        fmt::print("  seq: {}\n", seq_out_path);

    //Tokenize File with word IDs
    auto start_tokenize = std::chrono::high_resolution_clock::now();
    auto hashed_words = tokenize_file(input_path);
    auto end_tokenize = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> tokenize_duration = end_tokenize - start_tokenize;
    printf("Tokenize File Duration:\t\t\t%f s\n", tokenize_duration.count());

    // Windowed Serial Execution Steps
    auto start_serial = std::chrono::high_resolution_clock::now();
    //auto counts = seq_count_words(hashed_words);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> serial_count_duration = end_serial - start_serial;
    printf("Seq Count Words Duration:\t\t\t%f s\n", serial_count_duration.count());

    // Windowed Parallel Execution Steps
    auto start_par = std::chrono::high_resolution_clock::now();
    sub_buffer_count_words(hashed_words);
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> par_count_duration = end_par - start_par;
    printf("Parallel Count Words Duration:\t\t\t%f s\n", par_count_duration.count());
}