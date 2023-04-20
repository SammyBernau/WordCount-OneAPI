#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>
using namespace std;


int sha256(const string &w) {
  unsigned char sha256_hash[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha256_hash);
  string sha256_str(reinterpret_cast<const char *>(sha256_hash),
                    SHA256_DIGEST_LENGTH);
  return hash<string>{}(sha256_str);
}

// Implemented sha512 because md5 is considered less effective
int sha512(const string &w) {
  unsigned char sha512_hash[SHA512_DIGEST_LENGTH];
  SHA512(reinterpret_cast<const unsigned char *>(w.c_str()), w.length(),
         sha512_hash);
  string sha512_str(reinterpret_cast<const char *>(sha512_hash),
                    SHA512_DIGEST_LENGTH);
  return hash<string>{}(sha512_str);
}