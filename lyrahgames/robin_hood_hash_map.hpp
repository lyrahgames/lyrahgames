#pragma once
#include <functional>
#include <vector>

namespace lyrahgames {

class robin_hood_hash_map {
 public:
  using key_type = int;
  using mapped_type = int;
  using flag_type = size_t;

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

  robin_hood_hash_map() = default;

  static constexpr size_t hash(key_type key) noexcept { return key; }

  bool empty() const noexcept { return _load == 0; }
  size_t size() const noexcept { return _load; }
  size_t capacity() const noexcept { return _table.size; }

  std::pair<size_t, size_t> key_swap_index(const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.flags[index]; ++psl) {
      if (_table.keys[index] == key) return {index, psl};
      index = (index + 1) & _capacity_mask;
    }
    return {index, psl};
  }

  std::pair<size_t, size_t> new_key_swap_index(
      const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.flags[index]; ++psl) {
      index = (index + 1) & _capacity_mask;
    }
    return {index, psl};
  }

  void insert_key_by_swap(const key_type& key, size_t index, size_t psl) {
    if (!_table.flags[index]) {
      _table.keys[index] = key;
      // _table.values[index] = std::move(tmp_value);
      _table.flags[index] = psl;
      return;
    }

    key_type tmp_key = std::move(_table.keys[index]);
    mapped_type tmp_value = std::move(_table.values[index]);
    _table.keys[index] = key;
    // _table.values[index] = mapped_type{};
    std::swap(psl, _table.flags[index]);
    ++psl;
    index = (index + 1) & _capacity_mask;

    for (; _table.flags[index]; ++psl) {
      if (psl > _table.flags[index]) {
        std::swap(psl, _table.flags[index]);
        std::swap(tmp_key, _table.keys[index]);
        std::swap(tmp_value, _table.values[index]);
      }
      index = (index + 1) & _capacity_mask;
    }

    _table.keys[index] = std::move(tmp_key);
    _table.values[index] = std::move(tmp_value);
    _table.flags[index] = psl;
  }

  void double_capacity_and_rehash() {
    container old_table{_table.size << 1u};
    _table.swap(old_table);
    _capacity_mask = _table.size - 1u;
    for (size_t i = 0; i < old_table.size; ++i) {
      if (old_table.flags[i]) {
        const auto [index, psl] = new_key_swap_index(old_table.keys[i]);
        insert_key_by_swap(old_table.keys[i], index, psl);
        _table.values[index] = old_table.values[i];
      }
    }
  }

  mapped_type& operator[](const key_type& key) {
    // Try to find the element.
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.flags[index]; ++psl) {
      if (_table.keys[index] == key) return _table.values[index];
      index = (index + 1) & _capacity_mask;
    }

    // Could not find the element. So insert it by Robin-Hood swaping.
    ++_load;
    if (_load >= _max_load_factor * _table.size) {
      double_capacity_and_rehash();
      const auto [tmp_index, tmp_psl] = new_key_swap_index(key);
      index = tmp_index;
      psl = tmp_psl;
    }

    insert_key_by_swap(key, index, psl);
    return _table.values[index];
  }

  mapped_type* find(const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.flags[index]; ++psl) {
      if (_table.keys[index] == key) return &_table.values[index];
      index = (index + 1) & _capacity_mask;
    }
    return nullptr;
  }

  void erase_by_swap(size_t index) {
    size_t next_index = (index + 1) & _capacity_mask;
    while (_table.flags[next_index] > 1) {
      _table.keys[index] = std::move(_table.keys[next_index]);
      _table.values[index] = std::move(_table.values[next_index]);
      _table.flags[index] = _table.flags[next_index] - 1;
      index = next_index;
      next_index = (next_index + 1) & _capacity_mask;
    }
    _table.flags[index] = 0;
    // destroy key and value
  }

  bool erase(const key_type& key) noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.flags[index]; ++psl) {
      if (_table.keys[index] == key) {
        erase_by_swap(index);
        --_load;
        return true;
      }
      index = (index + 1) & _capacity_mask;
    }
    return false;
  }

  container _table{8};
  size_t _load{};
  size_t _capacity_mask{7};
  float _max_load_factor{0.8};
};

}  // namespace lyrahgames