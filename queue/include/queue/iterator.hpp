#pragma once
#include <iterator>

namespace con {
template <class T> class rnd_iterator {
  T *m_Ptr;

public:
  /*
   * type aliases
   */
  using value_type = T;
  using iterator_type = rnd_iterator<T>;
  using iterator_category = std::random_access_iterator_tag;
  using pointer = value_type *;
  using const_pointer = const pointer;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const reference;
  /*
   * constructors
   */
  rnd_iterator() noexcept : m_Ptr(nullptr) {}
  explicit rnd_iterator(const rnd_iterator<T> &other) noexcept
      : m_Ptr(other.m_Ptr) {}
  explicit rnd_iterator(pointer p) noexcept : m_Ptr(p) {}
  /*
   * access operators
   */
  reference operator*() { return *m_Ptr; }
  reference operator[](std::size_t idx) { return m_Ptr[idx]; }
  pointer operator->() { return m_Ptr; }
  /*
   * increment/decrement and assign operators
   */
  rnd_iterator &operator=(pointer oth) {
    m_Ptr = oth;
    return *this;
  }
  rnd_iterator &operator+=(pointer oth) {
    m_Ptr += oth;
    return *this;
  }
  rnd_iterator &operator-=(pointer oth) {
    m_Ptr -= oth;
    return *this;
  }
  rnd_iterator operator++() {
    ++m_Ptr;
    return *this;
  }
  rnd_iterator operator++(int) {
    auto temp = *this;
    m_Ptr++;
    return temp;
  }
  rnd_iterator &operator--() {
    --m_Ptr;
    return *this;
  }
  rnd_iterator operator--(int) {
    auto temp = *this;
    m_Ptr--;
    return temp;
  }
};
} // namespace con
