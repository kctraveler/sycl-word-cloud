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
#include "CLI/CLI.hpp"
#include "fmt/format.h"


// std::vector<std::map<int, short>> seq_count_words(std::vector<int> hashed_words, int WINDOW_SIZE){
//     std::vector<std::map<int, short>> seq_windows{};
//     for (int i = 0; i <= hashed_words.size() / WINDOW_SIZE; i += 1){
//         std::vector<short> window_counts(WORD_ID_RANGE, 0);
//         int start = i * WINDOW_SIZE;
//         int end = start + WINDOW_SIZE;
//         for (int j = start; j < end && j < hashed_words.size(); j++){
//             window_counts[hashed_words[j]] += 1;
//         }
//         auto count_map = get_word_id_counts(window_counts);
//         seq_windows.push_back(count_map);
//     }
//    return seq_windows;
// }

std::vector<std::vector<short>> seq_count_words(std::vector<int> hashed_words, int WINDOW_SIZE){
    std::vector<std::vector<short>> seq_windows{};
    for (int i = 0; i <= hashed_words.size() / WINDOW_SIZE; i += 1){
        std::vector<short> window_counts(WORD_ID_RANGE, 0);
        int start = i * WINDOW_SIZE;
        int end = start + WINDOW_SIZE;
        for (int j = start; j < end && j < hashed_words.size(); j++){
            window_counts[hashed_words[j]] += 1;
        }
        //auto count_map = get_word_id_counts(window_counts);
        seq_windows.push_back(window_counts);
    }
   return seq_windows;
}

void sub_buffer_count_words(std::vector<int> int_hashed_words, int WINDOW_SIZE){
    // Getting error below. Was introduced when I changed data type of buffer from size_t to int. With window size as a param, found that the error is not just related to data type but window size might be a bigger factor.
    // "Specified offset of the sub-buffer being constructed is not a multiple of the memory base address alignment"
    // Further testing reveals that power of 2 window size seems to work with wide success. Weird that 10,000 worked with size_t but 5,000 did not.
    std::vector<size_t> hashed_words(int_hashed_words.begin(), int_hashed_words.end()); // change back to size_t
    //std::vector<int> hashed_words{int_hashed_words.begin(), int_hashed_words.end() }; // base address alignment error

    sycl::queue q;
    //sycl::buffer<int> b_hash_words{hashed_words};// base address alignment error
    sycl::buffer<size_t> b_hash_words{hashed_words}; 
    int N = hashed_words.size();
    size_t num_sub_buffs = N / WINDOW_SIZE + 1;
    sycl::buffer<int, 2> results(sycl::range{num_sub_buffs, WORD_ID_RANGE});
    std::cout << "BufferSize  " << b_hash_words.size() << std::endl;
    for (size_t i = 0; i < num_sub_buffs; i++){
        size_t start = i * WINDOW_SIZE;
        size_t end = WINDOW_SIZE;
        std::cout << "Start  " << start << std::endl;
        // limits last window to remaining elements
        // Why are the remainders here never an issue for address alignment?
        if (end + start >= N) {
            end = N - start;
        }
        std::cout << "Range Size " << end << std::endl;
        //sycl::buffer<int> sub_buff(b_hash_words, start, sycl::range{end}); //base address alignment error
        sycl::buffer<size_t> sub_buff(b_hash_words, start, sycl::range{end}); 
        // Using a sub buffer here was causing errors and not writing back to the original. Only the index 0 of outer dim was updated.
        //sycl::buffer<int, 2> sub_results(results, sycl::id{i, 0}, sycl::range{1, WORD_ID_RANGE});
        q.submit([&](sycl::handler& h){
            sycl::accessor in{sub_buff, h, sycl::read_only};
            sycl::accessor out{results, h};
            h.parallel_for(end, [=](sycl::id<1> idx) {
                out[i][in[idx]] += 1;
            });
        });
        
    }
    sycl::host_accessor counts(results, sycl::read_only);
    for (int i = 0; i < num_sub_buffs; i++){
        for (int j = 0; hashed_words.size(); j++){
            if (counts[i][hashed_words[j]] > 0 ){
                std::cout << "Window: " << i;
                std::cout << " Smallest word ID found: " << hashed_words[j];
                std::cout << " Count: " << counts[i][hashed_words[j]] << std::endl;
                break;
            }
        }    
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
    auto counts = seq_count_words(hashed_words, WINDOW_SIZE);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> serial_count_duration = end_serial - start_serial;
    printf("Seq Count Words Duration:\t\t\t%f s\n", serial_count_duration.count());

    // Windowed Parallel Execution Steps
    auto start_par = std::chrono::high_resolution_clock::now();
    sub_buffer_count_words(hashed_words, WINDOW_SIZE);
    auto end_par = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> par_count_duration = end_par - start_par;
    printf("Parallel Count Words Duration:\t\t\t%f s\n", par_count_duration.count());
}