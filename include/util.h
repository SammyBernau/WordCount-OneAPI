#include <string>
#include <set>
#include <vector>
#include <CL/sycl.hpp>

using namespace std;

vector<string> file_to_vec(string);
vector<int> file_to_vec_hash(string);

int sha256(const string &w);
int sha512(const char* w);
