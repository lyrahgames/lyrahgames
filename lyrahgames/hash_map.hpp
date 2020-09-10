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

  static constexpr size_t hash(key_type key) noexcept { return key; }

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
    auto index = hash(key) & capacity_mask;
    while ((_table.flags[index]) && (_table.keys[index] != key))
      index = (index + 1) & capacity_mask;
    return index;
  }

  size_t key_insert_index(key_type key) const noexcept {
    const auto capacity_mask = _table.size - 1u;
    size_t index = hash(key) & capacity_mask;
    size_t deleted_index = -1u;
    while ((_table.flags[index]) && (_table.keys[index] != key)) {
      if ((deleted_index == -1u) && (_table.flags[index] == 2))
        deleted_index = index;
      index = (index + 1) & capacity_mask;
    }
    return (!_table.flags[index] && (deleted_index != -1u)) ? (deleted_index)
                                                            : (index);
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

  // void insert(std::pair<key_type, mapped_type> n) {
  //   const auto index = key_index(n.first);
  //   if (!_table.flags[index]) ++_load;
  //   _table.keys[index] = n.first;
  //   _table.values[index] = n.second;
  //   _table.flags[index] = 1;
  //   if (_load >= _max_load_factor * _table.size)
  //   double_capacity_and_rehash();
  // }

  // void insert(const key_type& key, const mapped_type& value) {
  //   const auto index = key_insert_index(key);
  //   if (_table.flags[index] != 1) {
  //     ++_load;
  //     if (_table.flags[index] == 2) --_tombstones;
  //     _table.keys[index] = key;
  //     _table.flags[index] = 1;
  //     if (_load >= _max_load_factor * _table.size)
  //     double_capacity_and_rehash();
  //   }
  //   _table.values[index] = value;
  // }

  mapped_type& operator[](const key_type& key) {
    const auto index = key_insert_index(key);
    if (_table.flags[index] != 1) {
      ++_load;
      _tombstones = _tombstones - (_table.flags[index] == 2);

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

  mapped_type* find(const key_type& key) noexcept {
    const auto index = key_index(key);
    if (!_table.flags[index]) return nullptr;
    return &_table.values[index];
  }

  bool erase(const key_type& key) {
    const auto index = key_index(key);
    if (!_table.flags[index]) return false;
    _table.keys[index] = -1;
    _table.values[index] = -1;
    _table.flags[index] = 2;
    ++_tombstones;
    --_load;
    return true;
  }

  container _table{8};
  size_t _load{};
  size_t _tombstones{};
  float _max_load_factor{0.8};
};

}  // namespace lyrahgames