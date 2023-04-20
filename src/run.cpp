//#include "../include/hash.h"
#include <iostream>
#include <fstream>
#include <map>
#include <bits/stdc++.h>
#include <set>
#include <array>
#include <vector>
#include <algorithm>
#include<string>
#include <sycl/sycl.hpp>
#include <unordered_map>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

using namespace sycl;
using namespace std;


//START OF HELPER FUNCTIONS 

// get dictionary as vec
set<string> file_to_set(string input_file) {
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

  return words;
}

int sha256(const string &w) {
  unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha256_hash);
  string sha256_str(reinterpret_cast<const char *>(sha256_hash),
                    SHA256_DIGEST_LENGTH);
  return hash<string>{}(sha256_str);
}

int sha512(const string &w) {
  unsigned char sha512_hash[SHA512_DIGEST_LENGTH];
  SHA512(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha512_hash);
  string sha512_str(reinterpret_cast<const char *>(sha512_hash),
                    SHA512_DIGEST_LENGTH);
  return hash<string>{}(sha512_str);
}

//END OF HELPER FUNCTIONS 

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





// map<string, int> word_count_parallel_attempt1(queue &q, set<string> &words) {
//   map<string, int> word_count;

//   range num_words{words.size()};

//   // Create a buffer to hold the word set
//   buffer<char, 1> word_buf(words.begin(), words.end());

//   // Create a buffer to hold the hashed values
//   buffer<int, 1> hash_buf(num_words);

//   // Submit a kernel to hash each word
//   q.submit([&] (auto &h) {
//     accessor words_acc(word_buf, h, read_only);
//     accessor hash_acc(hash_buf, h, write_only);

//     h.parallel_for(num_words, [=] (const auto& idx) {
//       const auto& word = words_acc[idx];
//       int hashed_word = sha256(word) + sha512(word);
//       hash_acc[idx] = hashed_word;
//     });
//   });

//   // Copy the hashed values back to the host and insert them into the map
//   vector<int> hash_vec(num_words);
//   copy(hash_buf, hash_vec.begin());
//   for (size_t i = 0; i < num_words.size(); ++i) {
//     word_count[words[i]] = hash_vec[i];
//   }

//   return word_count;
// }

unordered_map<string, int> hash_words(sycl::queue& q, const set<string>& words) {
  // Create a map to store the hashed words
  unordered_map<string, int> word_map;


  //Convert the set to a vector for buffer creation
  vector<string> words_vec(words.begin(), words.end());

  // Create a buffer to hold the words
  buffer<char> words_buf{words_vec};

  // Create a buffer to hold the hashed words
  buffer<int> hash_buf{words.size()};

  // Submit a command group to the queue to hash the words
  q.submit([&](auto &h ){
    // Get access to the buffers
    accessor words_acc (words_buf, h, read_only, no_init);
    accessor hash_acc(hash_buf, h, write_only, no_init);

    // Define a kernel to hash each word and store it in the hash buffer
    h.parallel_for(range<1>{words.size()}, [=](item<1> idx) {
      hash_acc[idx] = sha512(words_acc[idx]) + sha256(words_acc[idx]);
      // Store the word and hashed value in the map
      word_map.insert({words_acc[idx], hash_acc[idx]});
    });
  });

  // Wait for the queue to finish
  q.wait();

  // Return the map
  return word_map;
}



int main() {
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

unordered_map<string,int> word_count;

try {
  sycl::queue q(selector, exception_handler);
  

  string file = "hamlet.txt";
  set words_as_set = file_to_set(file);
  
  cout << "Running on device: "<< q.get_device().get_info<info::device::name>() << "\n";

  word_count = hash_words(q, words_as_set);

  } catch (std::exception const &e) {
    cout << "An exception is caught while computing on device.\n";
    terminate();
  }


  for (const auto& word: word_count) {
    cout << "Key: " << word.first << ", Value: " << word.second << std::endl;
  }
}