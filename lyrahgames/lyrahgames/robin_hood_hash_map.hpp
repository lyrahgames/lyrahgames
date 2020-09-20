#pragma once
#include <functional>
#include <stdexcept>
#include <vector>

namespace lyrahgames::robin_hood {

template <typename Key, typename T, typename Hash = std::hash<Key>,
          typename Key_equal = std::equal_to<Key>,
          typename Allocator = std::allocator<Key>>
class hash_map {
 public:
  using key_type = Key;
  using mapped_type = T;
  using hasher = Hash;
  using key_equal = Key_equal;
  // using value_type = std::pair<const Key, T>;
  using allocator_type = Allocator;

  struct container {
    container(size_t n = 0) : size{n} {
      keys = new key_type[size];
      values = new mapped_type[size];
      psl = new size_t[size]{0};
      // typename std::allocator_traits<allocator_type>::rebind_alloc<key_type>
      //     key_alloc{};
      // keys =
      //     std::allocator_traits<decltype(key_alloc)>::allocate(key_alloc,
      //     size);
      // typename
      // std::allocator_traits<allocator_type>::rebind_alloc<mapped_type>
      //     value_alloc{};
      // values = std::allocator_traits<decltype(value_alloc)>::allocate(
      //     value_alloc, size);
      // typename std::allocator_traits<allocator_type>::rebind_alloc<size_t>
      //     psl_alloc{};
      // psl =
      //     std::allocator_traits<decltype(psl_alloc)>::allocate(psl_alloc,
      //     size);
      // for (size_t i = 0; i < size; ++i) psl[i] = 0;
    }

    ~container() {
      delete[] keys;
      delete[] values;
      delete[] psl;
      // typename std::allocator_traits<allocator_type>::rebind_alloc<key_type>
      //     key_alloc{};

      // std::allocator_traits<decltype(key_alloc)>::deallocate(key_alloc, keys,
      //                                                        size);
      // typename
      // std::allocator_traits<allocator_type>::rebind_alloc<mapped_type>
      //     value_alloc{};
      // std::allocator_traits<decltype(value_alloc)>::deallocate(value_alloc,
      //                                                          values, size);
      // typename std::allocator_traits<allocator_type>::rebind_alloc<size_t>
      //     psl_alloc{};
      // std::allocator_traits<decltype(psl_alloc)>::deallocate(psl_alloc, psl,
      //                                                        size);
    }

    container(const container&) = delete;
    container& operator=(const container&) = delete;
    container(container&&) = delete;
    container& operator=(container&&) = delete;

    void swap(container& c) noexcept {
      std::swap(keys, c.keys);
      std::swap(values, c.values);
      std::swap(psl, c.psl);
      std::swap(size, c.size);
    }

    key_type* keys{};
    mapped_type* values{};
    size_t* psl{};
    size_t size{};
  };

  template <bool Constant>
  struct iterator_t {
    using reference =
        std::conditional_t<Constant,
                           std::pair<const key_type&, const mapped_type&>,
                           std::pair<const key_type&, mapped_type&>>;

    using container_pointer =
        std::conditional_t<Constant, const container*, container*>;

    iterator_t& operator++() {
      do {
        ++index;
      } while ((index < base->size) && !base->psl[index]);
      return *this;
    }
    iterator_t operator++(int) {
      auto ip = *this;
      ++(*this);
      return ip;
    }
    reference operator*() const {
      return {base->keys[index], base->values[index]};
    }
    bool operator==(iterator_t it) const noexcept {
      // return (base == it.base) && (index == it.index);
      return index == it.index;
    }
    bool operator!=(iterator_t it) const noexcept {
      // return (index != it.index) || (base != it.base);
      return index != it.index;
    }

    container_pointer base{nullptr};
    size_t index{0};
  };

  using iterator = iterator_t<false>;
  using const_iterator = iterator_t<true>;

  hash_map() = default;
  // hash_map(std::initializer_list<std::pair<key_type, value_type>> list)
  //     : _table{list.size() / load_factor()} {
  //   insert(list.begin(), list.end());
  // }

  bool empty() const noexcept { return _load == 0; }
  size_t size() const noexcept { return _load; }
  size_t capacity() const noexcept { return _table.size; }
  float load_factor() const noexcept { return float(_load) / _table.size; }
  float max_load_factor() const noexcept { return _max_load_factor; }
  void max_load_factor(float f) {
    if ((f <= 0.0f) || (f >= 1.0f))
      throw std::out_of_range(
          "max_load_factor has to be a number between 0.0f and 1.0f.");
    _max_load_factor = f;
  }

  iterator begin() noexcept {
    for (size_t i = 0; i < _table.size; ++i)
      if (_table.psl[i]) return {&_table, i};
    return {&_table, _table.size};
  }
  const_iterator begin() const noexcept {
    for (size_t i = 0; i < _table.size; ++i)
      if (_table.psl[i]) return {&_table, i};
    return {&_table, _table.size};
  }
  // const_iterator cbegin() const noexcept;
  iterator end() noexcept { return {&_table, _table.size}; }
  const_iterator end() const noexcept { return {&_table, _table.size}; }
  // const_iterator cend() const noexcept;

  std::pair<size_t, size_t> key_swap_index(const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) return {index, psl};
      index = (index + 1) & _capacity_mask;
    }
    return {index, psl};
  }

  std::pair<size_t, size_t> new_key_swap_index(
      const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      index = (index + 1) & _capacity_mask;
    }
    return {index, psl};
  }

  void insert_key_by_swap(const key_type& key, size_t index, size_t psl) {
    if (!_table.psl[index]) {
      _table.keys[index] = key;
      // _table.values[index] = std::move(tmp_value);
      _table.psl[index] = psl;
      return;
    }

    key_type tmp_key = std::move(_table.keys[index]);
    mapped_type tmp_value = std::move(_table.values[index]);
    _table.keys[index] = key;
    // _table.values[index] = mapped_type{};
    std::swap(psl, _table.psl[index]);
    ++psl;
    index = (index + 1) & _capacity_mask;

    for (; _table.psl[index]; ++psl) {
      if (psl > _table.psl[index]) {
        std::swap(psl, _table.psl[index]);
        std::swap(tmp_key, _table.keys[index]);
        std::swap(tmp_value, _table.values[index]);
      }
      index = (index + 1) & _capacity_mask;
    }

    _table.keys[index] = std::move(tmp_key);
    _table.values[index] = std::move(tmp_value);
    _table.psl[index] = psl;
  }

  void double_capacity_and_rehash() {
    container old_table{_table.size << 1u};
    _table.swap(old_table);
    _capacity_mask = _table.size - 1u;
    for (size_t i = 0; i < old_table.size; ++i) {
      if (old_table.psl[i]) {
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
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) return _table.values[index];
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

  mapped_type& at(const key_type& key) {
    // Try to find the element.
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) return _table.values[index];
      index = (index + 1) & _capacity_mask;
    }
    // Could not find the element. So insert it by Robin-Hood swaping.
    throw std::out_of_range("Given element could not be found in hash_map.");
  }

  const mapped_type& at(const key_type& key) const {
    return const_cast<hash_map&>(*this).at(key);
  }

  // mapped_type* find(const key_type& key) const noexcept {
  //   size_t index = hash(key) & _capacity_mask;
  //   size_t psl = 1;
  //   for (; psl <= _table.psl[index]; ++psl) {
  //     if (equal_to(_table.keys[index], key)) return &_table.values[index];
  //     index = (index + 1) & _capacity_mask;
  //   }
  //   return nullptr;
  // }

  iterator find(const key_type& key) noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) return {&_table, index};
      index = (index + 1) & _capacity_mask;
    }
    return end();
  }

  const_iterator find(const key_type& key) const noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) return {&_table, index};
      index = (index + 1) & _capacity_mask;
    }
    return end();
  }

  void erase_by_swap(size_t index) {
    size_t next_index = (index + 1) & _capacity_mask;
    while (_table.psl[next_index] > 1) {
      _table.keys[index] = std::move(_table.keys[next_index]);
      _table.values[index] = std::move(_table.values[next_index]);
      _table.psl[index] = _table.psl[next_index] - 1;
      index = next_index;
      next_index = (next_index + 1) & _capacity_mask;
    }
    _table.psl[index] = 0;
    // destroy key and value
  }

  bool erase(const key_type& key) noexcept {
    size_t index = hash(key) & _capacity_mask;
    size_t psl = 1;
    for (; psl <= _table.psl[index]; ++psl) {
      if (equal_to(_table.keys[index], key)) {
        erase_by_swap(index);
        --_load;
        return true;
      }
      index = (index + 1) & _capacity_mask;
    }
    return false;
  }

  template <typename InputIt>
  void insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
      const auto& [key, value] = *it;
      operator[](key) = value;
    }
  }

  template <typename KeyInputIt, typename MappedInputIt>
  void insert(KeyInputIt first, KeyInputIt last, MappedInputIt value_first) {
    auto it = first;
    auto value_it = value_first;
    for (; it != last; ++it, ++value_it) {
      operator[](*it) = *value_it;
    }
  }

  container _table{8};
  hasher hash{};
  key_equal equal_to{};

  size_t _load{};
  size_t _capacity_mask{7};
  float _max_load_factor{0.8};
};

}  // namespace lyrahgames::robin_hood