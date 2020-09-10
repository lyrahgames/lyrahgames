#include <cassert>
#include <iomanip>
#include <iostream>
#include <lyrahgames/robin_hood_hash_map.hpp>
#include <string>

using namespace std;

template <typename Key, typename T, typename Hash, typename Key_equal>
ostream& operator<<(
    ostream& os,
    lyrahgames::robin_hood::hash_map<Key, T, Hash, Key_equal>& map) {
  for (size_t i = 0; i < map._table.size; ++i) {
    os << setw(5) << i << ":" << setw(8) << map._table.keys[i] << " ->"
       << setw(5) << map._table.values[i] << " :" << setw(5)
       << (int)map._table.psl[i] << '\n';
  }
  return os;
}

template <typename Key>
struct trivial_hash {
  constexpr size_t operator()(const Key& key) const noexcept {
    return static_cast<size_t>(key);
  }
};

int main() {
  using lyrahgames::robin_hood::hash_map;

  {
    hash_map<int, int, trivial_hash<int>> map{};
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
    // map[32] = 7;
    // cout << map << '\n';
    map.erase(1);
    cout << map << '\n';
  }

  {
    hash_map<std::string, int> map{};
    cout << map << '\n';
    map["test"] = 1;
    map["helo"] = 2;
    map["cd"] = 3;
    map["cp"] = 4;
    map["ls"] = 5;
    map["tree"] = 6;
    map["cat"] = 7;
    map["mkdir"] = 8;
    map["rm"] = 9;
    map["ls"] = 10;
    map["b"] = 11;
    map["bdep"] = 12;
    map["g++"] = 13;
    map["clang"] = 14;
    map["make"] = 15;
    map["bpkg"] = 16;
    map["bash"] = 17;
    map["fish"] = 18;
    map["top"] = 19;
    map["htop"] = 20;
    map["git"] = 21;
    map["vim"] = 22;
    map["touch"] = 23;
    map["rmdir"] = 24;
    map["sudo"] = 25;
    map["nano"] = 26;
    cout << map << '\n';
    map.erase("bpkg");
    cout << map << '\n';
  }
}