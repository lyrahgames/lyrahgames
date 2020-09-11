#include <doctest/doctest.h>

#include <iostream>
#include <lyrahgames/robin_hood_hash_map.hpp>
#include <random>

using namespace std;
using lyrahgames::robin_hood::hash_map;

TEST_CASE("A simple hash map") {
  hash_map<int, int> map{};
  CHECK(map.size() == 0);

  mt19937 rng{random_device{}()};

  SUBCASE("can insert and access values through the subscript operator '[]'.") {
    map[0] = 1;
    CHECK(map.size() == 1);
    CHECK(map[0] == 1);
    CHECK(map.size() == 1);

    map[1] = 3;
    CHECK(map.size() == 2);
    CHECK(map[1] == 3);
    CHECK(map.size() == 2);
  }

  SUBCASE("will rehash the values if the load factor is too big.") {
    map.max_load_factor(0.7);
    CHECK(map.max_load_factor() == 0.7f);

    CHECK(map.size() == 0);
    CHECK(map.load_factor() == 0.0f);

    const auto old_capacity = map.capacity();
    while (map.capacity() == old_capacity) {
      map[rng()] = rng();
      CHECK(map.load_factor() < map.max_load_factor());

      cout << "{ ";
      for (auto [k, v] : map) cout << "(" << k << ", " << v << ") ";
      cout << "}\n";
    }

    CHECK(map.load_factor() < map.max_load_factor());
    CHECK(map.capacity() == 2 * old_capacity);
  }
}