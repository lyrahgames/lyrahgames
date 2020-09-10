#include <cassert>
#include <iomanip>
#include <iostream>
#include <lyrahgames/robin_hood_hash_map.hpp>

using namespace std;

ostream& operator<<(ostream& os, lyrahgames::robin_hood_hash_map& map) {
  for (size_t i = 0; i < map._table.size; ++i) {
    os << setw(5) << i << ":" << setw(5) << map._table.keys[i] << " ->"
       << setw(5) << map._table.values[i] << " :" << setw(5)
       << (int)map._table.flags[i] << '\n';
  }
  return os;
}

int main() {
  lyrahgames::robin_hood_hash_map map{};

  // assert(map.size() == 0);
  // assert(map.capacity() == 8);
  // assert(map.empty());
  // assert(map.find(5) == nullptr);

  // map.insert(0, 1);

  // assert(map.size() == 1);
  // assert(!map.empty());
  // assert(*map.find(0) == 1);
  // assert(map.find(4) == nullptr);

  // map.insert(3, 2);
  // map.insert(7, 3);
  // map.insert(24, 4);
  // map.insert(4, 5);
  // map.insert(8, 6);
  // map.insert(8, 7);
  // map.insert(123, 8);
  // map[24] = 4;
  // map[123] = 8;
  // map[8] = 6;
  // map[3] = 2;
  // map[5] = 5;
  // map[10] = 1;

  cout << map << '\n';
  map[1] = 1;
  cout << map << '\n';
  map[9] = 2;
  cout << map << '\n';
  map[16] = 3;
  cout << map << '\n';
  map[2] = 4;
  cout << map << '\n';
  map[24] = 5;
  cout << map << '\n';
  map[1] = 6;
  cout << map << '\n';
  map[6] = 8;
  cout << map << '\n';
  map[32] = 7;
  cout << map << '\n';
  map.erase(1);
  cout << map << '\n';
}