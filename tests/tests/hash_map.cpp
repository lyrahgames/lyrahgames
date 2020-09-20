#include <doctest/doctest.h>

#include <iostream>
#include <lyrahgames/robin_hood_hash_map.hpp>
#include <random>
#include <vector>

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

  SUBCASE(
      "can access const and non-const values through the member function 'at' "
      "which throws an exception if the given key was not already inserted.") {
    map[0] = 0;
    map[1] = 4;
    map[2] = 1;
    map[3] = 5;

    CHECK(map.size() == 4);

    CHECK(map.at(0) == 0);
    CHECK(map.at(1) == 4);
    CHECK(map.at(2) == 1);
    CHECK(map.at(3) == 5);
    CHECK_THROWS_AS(map.at(4), std::out_of_range);

    CHECK(map.size() == 4);

    map.at(2) = 3;
    CHECK(map.at(2) == 3);

    CHECK(map.size() == 4);

    const auto& map_ref = map;
    CHECK(map_ref.at(0) == 0);
    CHECK(map_ref.at(1) == 4);
    CHECK(map_ref.at(2) == 3);
    CHECK(map_ref.at(3) == 5);
    CHECK_THROWS_AS(map_ref.at(4), std::out_of_range);

    // map_ref.at(2) = 5;
  }
}

SCENARIO(
    "The hash map can insert and access one or more values through "
    "iterators.") {
  GIVEN("a default constructed hash map") {
    hash_map<int, int> map{};

    GIVEN("some random vector containing values with unique keys") {
      constexpr auto count = 1000;
      mt19937 rng{random_device{}()};

      vector<int> keys(count);
      iota(begin(keys), end(keys), -count / 2);
      shuffle(begin(keys), end(keys), rng);

      vector<int> values(count);
      generate(begin(values), end(values),
               bind(uniform_int_distribution<int>{-count, count}, ref(rng)));

      vector<pair<int, int>> data{};
      for (auto i = 0; i < count; ++i) data.push_back({keys[i], values[i]});

      WHEN("the values are inserted") {
        map.insert(begin(data), end(data));

        THEN(
            "for every value one gets a new element in the hash map with"
            " the same key and value") {
          CHECK(map.size() == data.size());
          for (const auto& element : data) {
            CHECK_MESSAGE(map.at(element.first) == element.second,
                          "element = (" << element.first << ", "
                                        << element.second << ")");
          }
        }
      }

      WHEN("keys and their values are inserted separately") {
        map.insert(begin(keys), end(keys), begin(values));

        THEN(
            "for every key-value-pair one gets a new key-value-element in the "
            "hash map.") {
          CHECK(map.size() == keys.size());
          for (auto i = 0; i < keys.size(); ++i) {
            CHECK_MESSAGE(map.at(keys[i]) == values[i],
                          "(keys[i], values[i]) = (" << keys[i] << ", "
                                                     << values[i] << ")");
          }
        }
      }
    }
  }

  GIVEN("a hash map with some initial data") {
    vector<pair<int, int>> data{{1, 1}, {2, 2}, {4, 4}, {5, 5}, {10, 10}};
    hash_map<int, int> map{};
    map.insert(begin(data), end(data));
    vector<pair<int, int>> read{};

    WHEN("one iterates with iterators and helper functions") {
      for (auto it = begin(map); it != end(map); ++it)
        read.push_back({(*it).first, (*it).second});
      THEN("every element in the hash map is reached in an undefined order") {
        sort(begin(read), end(read));
        CHECK(read == data);
      }
    }

    WHEN("one iterates with iterators and member functions") {
      for (auto it = map.begin(); it != map.end(); ++it)
        read.push_back({(*it).first, (*it).second});
      THEN("every element in the hash map is reached in an undefined order") {
        sort(begin(read), end(read));
        CHECK(read == data);
      }
    }

    WHEN("one iterates with a range-based for loop") {
      for (auto [key, value] : map) read.push_back({key, value});
      THEN("every element in the hash map is reached in an undefined order") {
        sort(begin(read), end(read));
        CHECK(read == data);
      }
    }

    GIVEN("a constant reference to this hash map") {
      const auto& map_ref = map;

      WHEN("one iterates with iterators and helper functions") {
        for (auto it = begin(map_ref); it != end(map_ref); ++it) {
          auto [key, value] = *it;
          read.push_back({key, value});
        }
        THEN("every element in the hash map is reached in an undefined order") {
          sort(begin(read), end(read));
          CHECK(read == data);
        }
      }

      WHEN("one iterates with iterators and member functions") {
        for (auto it = map_ref.begin(); it != map_ref.end(); ++it) {
          auto [key, value] = *it;
          read.push_back({key, value});
        }
        THEN("every element in the hash map is reached in an undefined order") {
          sort(begin(read), end(read));
          CHECK(read == data);
        }
      }

      WHEN("one iterates with a range-based for loop") {
        for (auto [key, value] : map_ref) read.push_back({key, value});
        THEN("every element in the hash map is reached in an undefined order") {
          sort(begin(read), end(read));
          CHECK(read == data);
        }
      }
    }

    SUBCASE(
        "One can access elements through the member function 'find' which "
        "returns an iterator to the element if it exists and the end iterator "
        "if it does not.") {
      {
        auto [key, value] = *map.find(1);
        CHECK(key == 1);
        CHECK(value == 1);
      }
      {
        auto [key, value] = *map.find(2);
        CHECK(key == 2);
        CHECK(value == 2);
      }
      {
        auto it = map.find(3);
        CHECK(it == end(map));
        CHECK(it == map.end());
      }

      GIVEN("a constant reference to this hash map") {
        const auto& map_ref = map;

        SUBCASE(
            "One can access elements through the overloaded member function "
            "'find' which returns a constant iterator to the element if it "
            "exists and the end-iterator if it does not.") {
          {
            auto [key, value] = *map_ref.find(1);
            CHECK(key == 1);
            CHECK(value == 1);
          }
          {
            auto [key, value] = *map_ref.find(2);
            CHECK(key == 2);
            CHECK(value == 2);
          }
          {
            auto it = map_ref.find(3);
            CHECK(it == end(map_ref));
            CHECK(it == map_ref.end());
          }
        }
      }
    }
  }
}

SCENARIO("The hash map can be initialized by initializer lists.") {
  WHEN("an initializer list with unique keys is used") {
    // hash_map<int, int> map{{1, 5}, {-1, 2}, {8, 4}, {5, -4}, {-3, -1}};
    // THEN("every key-value-pair can is inserted.") {
    //   CHECK(size(map) == 5);
    //   CHECK(map.at(1) == 5);
    //   CHECK(map.at(-1) == 2);
    //   CHECK(map.at(8) == 4);
    //   CHECK(map.at(5) == -4);
    //   CHECK(map.at(-3) == -1);
    // }
  }

  // WHEN("an initializer list with non-unique keys is used") {
  //   hash_map map{{1, 5}, {-1, 2}, {1, 4}, {5, -4}, {5, -1}};
  //   THEN("the mapped value of non-unique keys is one of the given values.") {
  //     CHECK(size(map) == 3);
  //     CHECK((map.at(1) - 5) * (map.at(1) - 4) == 0);
  //     CHECK(map.at(-1) == 2);
  //     CHECK((map.at(5) + 4) * (map.at(5) + 1) == 0);
  //   }
  // }
}