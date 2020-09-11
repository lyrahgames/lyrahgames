#include <doctest/doctest.h>

#include <lyrahgames/robin_hood_hash_map.hpp>
using lyrahgames::robin_hood::hash_map;

TEST_CASE("Test") {
  hash_map<int, int> map{};
  CHECK(map.size() == 0);
}