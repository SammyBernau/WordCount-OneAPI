#ifndef WORDCOUNT_H
#define WORDCOUNT_H

#include <vector>
#include <string>
#include <cmath>
#include <set>

using namespace std;

//Wrapper class that houses a set 
//We decided to create a class for this rather than just use set so that we could properly manage different instances of a word count without calling set a bunch of times
class WordCount {
public:
    WordCount(){};
    void insert(const string& word);
    int get_size();
    bool contains(const string& word);
    int get_total_words_seen();

private:
    set<uint64_t> words;
    int total_words_seen = 0;
    // int md5(const string& w);
    int sha256(const string& w);
    int sha512(const string& w);
};

#endif 