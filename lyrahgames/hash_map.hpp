#pragma once
#include <functional>
#include <vector>

namespace lyrahgames {

class hash_map {
 public:
  using key_type = int;
  using mapped_type = int;
  using flag_type = unsigned char;

  struct container {
    container(size_t n = 0) : size{n} {
      keys = new key_type[size];
      values = new mapped_type[size];
      flags = new flag_type[size]{0};
    }

    ~container() {
      delete[] keys;
      delete[] values;
      delete[] flags;
    }

    container(const container&) = delete;
    container& operator=(const container&) = delete;
    container(container&&) = delete;
    container& operator=(container&&) = delete;

    void swap(container& c) noexcept {
      std::swap(keys, c.keys);
      std::swap(values, c.values);
      std::swap(flags, c.flags);
      std::swap(size, c.size);
    }

    key_type* keys{};
    mapped_type* values{};
    flag_type* flags{};
    size_t size{};
  };

  hash_map() = default;

  // iterator begin() noexcept;
  // const_iterator begin() const noexcept;
  // const_iterator cbegin() const noexcept;
  // iterator end() noexcept;
  // const_iterator end() const noexcept;
  // const_iterator cend() const noexcept;

  bool empty() const noexcept { return _load == 0; }
  size_t size() const noexcept { return _load; }
  size_t capacity() const noexcept { return _table.size; }

  // void clear() noexcept;
  // insert alternatives: element, pair, array, iterator, node, hint
  // insert_or_assign
  // emplace
  // erase/extract

  // at
  // operator[]
  // find/contains

  size_t key_index(key_type key) const noexcept {
    const auto capacity_mask = _table.size - 1u;
    auto index = std::hash<key_type>{}(key)&capacity_mask;
    while ((_table.flags[index]) && (_table.keys[index] != key))
      index = (index + 1) & capacity_mask;
    return index;
  }

  // void rehash(size_t n) {}
  void double_capacity_and_rehash() {
    container old_table{_table.size << 1u};
    _table.swap(old_table);
    for (size_t i = 0; i < old_table.size; ++i) {
      if (old_table.flags[i]) {
        const auto index = key_index(old_table.keys[i]);
        _table.keys[index] = old_table.keys[i];
        _table.values[index] = old_table.values[i];
        _table.flags[index] = 1;
      }
    }
  }

  // void reserve(size_t n);
  // float load_factor() const noexcept;
  // float max_load_factor() const noexcept;
  // void max_load_factor(float m);

  void insert(std::pair<key_type, mapped_type> n) {
    const auto index = key_index(n.first);
    if (!_table.flags[index]) ++_load;
    _table.keys[index] = n.first;
    _table.values[index] = n.second;
    _table.flags[index] = 1;
    if (_load >= _max_load_factor * _table.size) double_capacity_and_rehash();
  }

  mapped_type& operator[](const key_type& key) {
    const auto index = key_index(key);
    if (!_table.flags[index]) {
      ++_load;
      _table.keys[index] = key;
      // _table.values[index] = {};
      _table.flags[index] = 1;
      if (_load >= _max_load_factor * _table.size) {
        double_capacity_and_rehash();
        // The index has been invalidated by rehashing.
        return _table.values[key_index(key)];
      }
    }
    return _table.values[index];
  }

  bool erase(const key_type& key) {
    const auto index = key_index(key);
    if (!_table.flags[index]) return false;
    _table.flags[index] = 2;
  }

  container _table{8};
  size_t _load{};
  float _max_load_factor{0.8};
};

}  // namespace lyrahgames