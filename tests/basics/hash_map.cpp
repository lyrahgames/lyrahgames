#include <cassert>
#include <iomanip>
#include <iostream>
#include <lyrahgames/hash_map.hpp>

using namespace std;

ostream& operator<<(ostream& os, lyrahgames::hash_map& map) {
  for (size_t i = 0; i < map._table.size; ++i) {
    os << setw(5) << map._table.keys[i] << ": " << map._table.values[i] << ": "
       << (int)map._table.flags[i] << '\n';
  }
  return os;
}

int main() {
  lyrahgames::hash_map map{};
  //   map.insert({0, 1});
  // map.insert({3, 2});
  // map.insert({7, 3});
  // map.insert({24, 4});
  // map.insert({4, 5});
  // map.insert({8, 6});
  // map.insert({8, 7});
  // map.insert({123, 8});
  map[24] = 4;
  map[123] = 8;
  map[8] = 6;
  map[3] = 2;
  map[5] = 5;
  map[10] = 1;
  cout << map << '\n';
}