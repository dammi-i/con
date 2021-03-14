#ifndef CON_ITERATOR_HPP
#define CON_ITERATOR_HPP
#include <compare>
#include <iterator>

namespace con {
template <class T> class rnd_iterator {

public:
  // clang-format off
  T* m_Ptr;
  /*
   * type aliases
   */
  // clang-format on
  using value_type = T;
  using iterator_type = rnd_iterator<value_type>;
  using iterator_category = std::random_access_iterator_tag;
  using pointer = value_type *;
  using const_pointer = const pointer;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  /*
   * constructors
   */
  explicit rnd_iterator(T *p) noexcept : m_Ptr(p) {}
  /*
   * access operators
   */
  reference operator*() { return *m_Ptr; }
  reference operator[](std::size_t idx) { return m_Ptr[idx]; }
  pointer operator->() { return m_Ptr; }
  /*
   * increment/decrement and assign operators
   */
  iterator_type &operator+=(difference_type n) {
    m_Ptr += n;
    return *this;
  }
  iterator_type &operator-=(difference_type n) {
    m_Ptr += n;
    return *this;
  }
  rnd_iterator &operator++() {
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

  constexpr auto operator<=>(const iterator_type &rhs) noexcept = default;

  friend iterator_type operator+(const iterator_type &it, difference_type n) {
    return iterator_type(it.m_Ptr + n);
  }
  friend iterator_type operator+(difference_type n, const iterator_type &it) {
    return iterator_type(it.m_Ptr + n);
  }
  friend iterator_type operator-(const iterator_type &it, difference_type n) {
    return iterator_type(it.m_Ptr - n);
  }
  friend iterator_type operator-(difference_type n, const iterator_type &it) {
    return iterator_type(it.m_Ptr - n);
  }
};
} // namespace con
#endif
