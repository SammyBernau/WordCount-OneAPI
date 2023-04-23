//Standard C++
#include <iostream>
#include <fstream>
#include <map>
#include <bits/stdc++.h>
#include <set>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <unordered_map>

//SYCL includes
#include <sycl/sycl.hpp>
#include <CL/sycl.hpp>

//My includes
#include "../include/CLI11.hpp"
#include "../include/util.h"

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

// void hash_words(sycl::queue& q, vector<string>& words_vec, vector<int>& hash_vec) {
//   //Converts vector<string> to vector<char> for compatibility with sycl::buffer
//   std::vector<char> words_chars;
//   for (const auto& word : words_vec) {
//     words_chars.insert(words_chars.end(), word.begin(), word.end());
//     // words_chars.push_back('\0');
//   }
  

//   //Create buffer to hold words from vector
//   sycl::buffer<char> words_buf(words_chars.data(), words_chars.size());


//   //Create a buffer to hold the hashes
//   sycl::buffer<int> hash_buf(hash_vec.data(), hash_vec.size());
  

//   //Submit a command group to compute hashes
//   q.submit([&](handler& h){

    
//     //Get accessors to the buffers
//     accessor words_acc(words_buf, h, read_write);

//     accessor hash_acc(hash_buf, h, write_only);
    
//     // Compute the hashes
//     h.parallel_for(range<1>{hash_buf.size()}, [=](id<1> i) {
      
//       char test[] = "test"; // compiles
      
//       hash_acc[i] = sha512(test);
      
//     });
//   });
//   q.wait();
// }

// unordered_map<string,int> form_map_result(vector<string>& words_as_vec, vector<int>& words_as_hash){

//   // Create a map to hold the results
//   unordered_map<string, int> result;

//   // Populate the map with the results
//   for (int i = 0; i < words_as_vec.size(); i++) {
//     result[words_as_vec[i]] = words_as_hash[i];
//   }

//   return result;
// }

// void contains_int(vector<int>& uniques, int& value, bool& found) {
//   // Create a queue to execute the kernel on a device
//   sycl::queue q;

//   sycl::buffer<int, 1> unique_buf(uniques.data(), uniques.size());
//   sycl::buffer<bool, 1> found_buf(&found, 1);
//   sycl::buffer<int, 1> value_buf(&value, 1);

//   // Submit a kernel to the queue that searches for the value in the buffer
//   //bool found = false;
//   q.submit([&](handler& h) {

//     accessor unique_acc(unique_buf, h, read_only);
//     accessor found_acc(found_buf, h, read_write);
//     accessor value_acc(value_buf, h, read_only);


//     h.parallel_for(range<1>{unique_acc.size()}, [=](id<1> idx) {
//       if (unique_acc[idx] == value_acc[0]) {
//         found_acc[0] = true;
//       }
//     });
//   });
// }

void remove_duplicates(sycl::queue& q, vector<int>& input_vec, vector<int>& output_vec, int& count) {
  
  // Create a SYCL buffer to hold the input vector
  sycl::buffer<int, 1> input_buf(input_vec.data(), input_vec.size());

  // Create a SYCL buffer to hold the output vector
  sycl::buffer<int, 1> output_buf(output_vec.data(), output_vec.size());

  //Create a SYCL buffer to hold the count of unique words
  sycl::buffer<int, 1> count_buf(&count, 1);
 

  // Submit a kernel to remove duplicates
  q.submit([&](handler& h) {
    // Create accessors for the input and output buffers
    accessor input_acc(input_buf, h, read_only);
    accessor output_acc(output_buf, h, write_only);
    accessor count_acc(count_buf, h, write_only);

    // Define the kernel
    h.parallel_for(range<1>(input_vec.size()), [=](sycl::item<1> item) { 
      int value = input_acc[item];

      bool is_duplicate = false;
      

      for (int k = 0; k < output_acc.size(); k++) {
          if(output_acc[k] == value) {
            is_duplicate = true;
          }
      }

      // If this value is not a duplicate, write it to the output buffer
      if (is_duplicate == false) {
        output_acc[item] = value;
        count_acc[0]++;
      }

    });
  });
}



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

//  vector<string> words_as_vec = file_to_vec(input_file);
//  vector<int> words_as_hash(words_as_vec.size());

vector<int> words_as_hash = file_to_vec_hash(input_file);
vector<int> output_vec(words_as_hash.size());
int count = 0;

try {
  sycl::queue q(selector, exception_handler);
  cout << "Running on device: "<< q.get_device().get_info<info::device::name>() << "\n";
  
  //hash_words(q, words_as_vec, words_as_hash);
  remove_duplicates(q, words_as_hash, output_vec, count); 

  } catch (std::exception const &e) {
    cout << "An exception is caught while computing on device.\n";
    terminate();
  }

  std::cout << "Number of unique words: " << count << std::endl;
  // unordered_map<string,int> word_count = form_map_result(words_as_vec, words_as_hash);

  // for (const auto& word: word_count) {
  //   cout << "Key: " << word.first << ", Value: " << word.second << std::endl;
  // }
}
