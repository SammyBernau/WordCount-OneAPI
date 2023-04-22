//#include "../include/hash.h"
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
// #include <openssl/evp.h>
// #include <openssl/md5.h>
// #include <openssl/sha.h>

using namespace sycl;
using namespace std;


//-------------------------------------START OF HELPER FUNCTIONS------------------------------------- 

//Gets a text file and inserts each individual word into a set
vector<string> file_to_vec(string input_file) {
  set<string> words;
  fstream new_file; // file object

  try {
    new_file.open(input_file, ios::in); // open file object for reading
    if (!new_file.is_open()) {
      cout << "Error opening file: " << input_file << std::endl;
    }
    if (new_file.is_open()) {
      string line;
      while (getline(new_file, line)) { // read file text
        string curr_word = "";
        for (auto &ch : line) {
          if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            curr_word += ch;
          }
            
          if (ch == ' ') {
            words.insert(curr_word);
            curr_word = "";
          }
        }

        if (curr_word.length() > 0) {
          words.insert(curr_word);
          curr_word = "";
        }
      }
      new_file.close();
    }
  } catch (const ios_base::failure &e) {
    cerr << "Error opening file: " << e.what() << std::endl;
  }
  std::cout << "Read file" << std::endl;

  std::vector<std::string> words_vec(words.begin(), words.end());

  return words_vec;
}

// //OpenSSL sha256 string to hash
// int my_sha256(const string &w) {
//   unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
//   SHA256(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
//          sha256_hash);
//   string sha256_str(reinterpret_cast<const char *>(sha256_hash),
//                     SHA256_DIGEST_LENGTH);
//   return hash<string>{}(sha256_str);
// }

// //OpenSSL sha512 string to hash
// int my_sha512(const string &w) {
//   unsigned char sha512_hash[SHA512_DIGEST_LENGTH];
//   SHA512(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
//          sha512_hash);
//   string sha512_str(reinterpret_cast<const char *>(sha512_hash),
//                     SHA512_DIGEST_LENGTH);
//   return hash<string>{}(sha512_str);
// }

//-------------------------------------END OF HELPER FUNCTIONS-------------------------------------

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

  std::vector<char> words_chars;
  for (const auto& word : words_vec) {
    words_chars.insert(words_chars.end(), word.begin(), word.end());
  }
  

  //Create buffer to hold words from set
  sycl::buffer<char> words_buf(words_chars.data(), words_chars.size());
  

  //Create a buffer to hold the hashes
  //range<1> num_words{words_vec.size()};
  sycl::buffer<int> hash_buf(hash_vec.data(), hash_vec.size());
  

  //Submit a command group to compute hashes
  q.submit([&](handler& h){

    //Get accessors to the buffers
    accessor words_acc(words_buf, h, read_only);
    accessor hash_acc(hash_buf, h, write_only);
  
    // Compute the hashes
    h.parallel_for(range<1>{hash_buf.size()}, [=](id<1> i) {
      // string char_as_string = words_acc[i];
      // hash_acc[i] = my_sha256(char_as_string) + my_sha512(char_as_string);
      hash_acc[i] = i;
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

  } catch (std::exception const &e) {
    cout << "An exception is caught while computing on device.\n";
    terminate();
  }

  unordered_map<string,int> word_count = form_map_result(words_as_vec, words_as_hash);

  for (const auto& word: word_count) {
    cout << "Key: " << word.first << ", Value: " << word.second << std::endl;
  }
}