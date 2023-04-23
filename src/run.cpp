#include "../include/util.h"
#include <iostream>
#include <fstream>
#include <map>
#include <bits/stdc++.h>
#include <set>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <sycl/sycl.hpp>
#include <unordered_map>
#include <CL/sycl.hpp>
#include "../include/CLI11.hpp"

using namespace sycl;
using namespace std;

// Create an exception handler for asynchronous SYCL exceptions
static auto exception_handler = [](sycl::exception_list e_list) {
  for (std::exception_ptr const &e : e_list) {
    try {
      std::rethrow_exception(e);
    }
    catch (std::exception const &e) {
#if _DEBUG
      std::cout << "Failure" << std::endl;
#endif
      std::terminate();
    }
  }
};

void hash_words(sycl::queue& q, vector<string>& words_vec, vector<int>& hash_vec) {

  //Converts vector<string> to vector<char> for compatibility with sycl::buffer
  std::vector<char> words_chars;
  for (const auto& word : words_vec) {
    words_chars.insert(words_chars.end(), word.begin(), word.end());
  }
  

  //Create buffer to hold words from vector
  sycl::buffer<char> words_buf(words_chars.data(), words_chars.size());

  // std::vector<sycl::buffer<char, 1>> word_buffers(words_vec.size());
  // for (int i = 0; i < words_vec.size(); ++i) {
  //   word_buffers[i] = sycl::buffer<char, 1>(words_vec[i].data(), sycl::range<1>(words_vec[i].size()));
  // }

  //Create a buffer to hold the hashes
  sycl::buffer<int> hash_buf(hash_vec.data(), hash_vec.size());
  

  //Submit a command group to compute hashes
  q.submit([&](handler& h){

    // std::vector<sycl::accessor<char, 1, sycl::access::mode::read_write>> word_accs;
    
    // for (int i = 0; i < word_buffers.size(); ++i) {
    // word_accs.push_back(word_buffers[i].get_access<sycl::access::mode::read_write>());
    // }

    //Get accessors to the buffers
    accessor words_acc(words_buf, h, read_write);

    accessor hash_acc(hash_buf, h, write_only);
    

    // Compute the hashes
    h.parallel_for(range<1>{hash_buf.size()}, [=](id<1> i) {
      std::string current_word(words_acc[i].get_pointer(), words_acc[i].get_pointer() + words_acc[i].get_count());

      // string current_word="";
      // for (int k = 0; k < current_word_array.size(); k++) {
      //   current_word += current_word_array[k];
      // }
      hash_acc[i] = sha256(current_word) + sha512(current_word);
    });
  });
  q.wait();
}

unordered_map<string,int> form_map_result(vector<string>& words_as_vec, vector<int>& words_as_hash){

  // Create a map to hold the results
  unordered_map<string, int> result;

  // Populate the map with the results
  for (int i = 0; i < words_as_vec.size(); i++) {
    result[words_as_vec[i]] = words_as_hash[i];
  }

  return result;
}



// void word_count(string file_path){

//   // Open file
//   std::ifstream file(file_path);

//   // Read file into buffer
//   std::string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//    buffer<char, 1> buf(contents.data(), range<1>(contents.size()), {property::buffer::use_host_ptr()});

//   // Count unique words
//   sycl::queue q;
//   std::vector<std::string> words;
//   int count = 0;
//   q.submit([&](handler& h) {
//     accessor acc (buf, h, read_only);
//     std::string word;
//     for (auto c : acc) {
//       if (c == ' ' || c == '\n') {
//         if (std::find(words.begin(), words.end(), word) == words.end()) {
//           words.push_back(word);
//           count++;
//         }
//         word.clear();
//       } else {
//         word += c;
//       }
//     }
//   });

//   q.wait();

//   // Print result
//   std::cout << "Number of unique words: " << count << std::endl;
// }



int main(int argc, char **argv) {
  CLI::App OneAPI_Word_Count{"Implementation of word count using intel's oneAPI"};

  string input_file;
    OneAPI_Word_Count.add_option("-i, --input_file", input_file, "Enter file path")
    -> required();

  // Create device selector for the device of your interest.
#if FPGA_EMULATOR
  // Intel extension: FPGA emulator selector on systems without FPGA card.
  auto selector = sycl::ext::intel::fpga_emulator_selector_v;
#elif FPGA_SIMULATOR
  // Intel extension: FPGA simulator selector on systems without FPGA card.
  auto selector = sycl::ext::intel::fpga_simulator_selector_v;
#elif FPGA_HARDWARE
  // Intel extension: FPGA selector on systems with FPGA card.
  auto selector = sycl::ext::intel::fpga_selector_v;
#else
  // The default device selector will select the most performant device.
  auto selector = default_selector_v;
#endif



CLI11_PARSE(OneAPI_Word_Count, argc, argv);

 vector<string> words_as_vec = file_to_vec(input_file);
 vector<int> words_as_hash(words_as_vec.size());

try {
  sycl::queue q(selector, exception_handler);
  cout << "Running on device: "<< q.get_device().get_info<info::device::name>() << "\n";
  
  hash_words(q, words_as_vec, words_as_hash);
  //word_count(input_file);

  } catch (std::exception const &e) {
    cout << "An exception is caught while computing on device.\n";
    terminate();
  }

  unordered_map<string,int> word_count = form_map_result(words_as_vec, words_as_hash);

  for (const auto& word: word_count) {
    cout << "Key: " << word.first << ", Value: " << word.second << std::endl;
  }
}
