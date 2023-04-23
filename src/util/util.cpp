#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
#include <functional>
#include <cstring>
//#include "../../include/utils.h"
using namespace std;



//OpenSSL implementation of sha256
int sha256(const string &w) {
  unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha256_hash);
  string sha256_str(reinterpret_cast<const char *>(sha256_hash),
                    SHA256_DIGEST_LENGTH);
  return hash<string>{}(sha256_str);
}

//OpenSSL implementation of sha512
int sha512(const string &w) {
  unsigned char sha512_hash[SHA512_DIGEST_LENGTH];
  SHA512(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha512_hash);
  string sha512_str(reinterpret_cast<const char *>(sha512_hash),
                    SHA512_DIGEST_LENGTH);
  return hash<string>{}(sha512_str);
}

//Gets a text file and inserts each individual word into a set then returns the result as a vector
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

vector<int> file_to_vec_hash(string input_file) {
  vector<int> hashes;
  fstream new_file; // file object

  try {
    new_file.open(input_file, ios::in); // open file object for reading
    if (!new_file.is_open()) {
      cout << "Error opening file: " << input_file << endl;
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
            int hash = sha256(curr_word) + sha512(curr_word);
            hashes.push_back(hash);
            curr_word = "";
          }
        }

        if (curr_word.length() > 0) {
          int hash = sha256(curr_word) + sha512(curr_word);
          hashes.push_back(hash);

          curr_word = "";
        }
      }
      new_file.close();
    }
  } catch (const ios_base::failure &e) {
    cerr << "Error opening file: " << e.what() << endl;
  }

  return hashes;

}

// int sha512(const char* w) {
//   unsigned char sha512_hash[SHA512_DIGEST_LENGTH];
//   SHA512(reinterpret_cast<const unsigned char *>(w), std::strlen(w),
//          sha512_hash);
//   string sha512_str(reinterpret_cast<const char *>(sha512_hash),
//                     SHA512_DIGEST_LENGTH);
//   return hash<string>{}(sha512_str);
// }

//C++ implentation of hash
int my_hash(const char* str) {
    std::hash<std::string> hasher;
    return static_cast<int>(hasher(str));
}
