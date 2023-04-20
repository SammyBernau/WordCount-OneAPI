#include <sycl/sycl.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
using namespace std;

// get dictionary as vec
set<string> file_to_map(string input_file) {
  set<string> words;
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
            words.push_back(curr_word);
            curr_word = "";
          }
        }

        if (curr_word.length() > 0) {
          words.push_back(curr_word);
          curr_word = "";
        }
      }
      new_file.close();
    }
  } catch (const ios_base::failure &e) {
    cerr << "Error opening file: " << e.what() << endl;
  }

  return words;
}