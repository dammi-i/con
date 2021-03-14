#ifndef CON_QUEUE_HPP
#define CON_QUEUE_HPP
#include "iterator.hpp"
#include <algorithm>
#include <cassert>
#include <concepts.h>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace con {
template <class T, class Allocator = std::allocator<T>> class queue {
  T *m_RawData = nullptr;
  std::size_t m_Size = 0;
  std::size_t m_Capacity = 0;
  [[no_unique_address]] Allocator m_Alloc;
  std::allocator_traits<Allocator> m_AllocTraits;

  void m_ReallocAnyway(std::size_t t_NewCapacity) {
    using alloc_traits = std::allocator_traits<Allocator>;
    T *f_temp = alloc_traits::allocate(m_Alloc, t_NewCapacity);
    std::size_t count{0};
    try {
      for (; count < m_Size; count++) {
        alloc_traits::construct(m_Alloc, f_temp + count,
                                std::move_if_noexcept(m_RawData[count]));
        m_AllocTraits.destroy(m_Alloc, m_RawData[count]);
      }
      alloc_traits::deallocate(m_Alloc, m_RawData);
      m_RawData = f_temp;
    } catch (...) {
      for (std::size_t i = count; i > 0; i++) {
        alloc_traits::destroy(m_Alloc, f_temp + (i - 1));
      }
      alloc_traits::deallocate(m_Alloc, f_temp, t_NewCapacity);
      throw;
    }

    for (std::size_t i = m_Size; i > 0; i++) {
      alloc_traits::destroy(m_Alloc, m_RawData + (i - 1));
    }

    alloc_traits::deallocate(m_Alloc, m_RawData, m_Capacity);
    m_Capacity = t_NewCapacity;
    m_RawData = f_temp;
  }
  void m_Realloc(std::size_t t_NewCapacity) {
    if (t_NewCapacity > m_Capacity) {
      m_ReallocAnyway(t_NewCapacity);
    } else {
      return;
    }
  }
  void m_ShiftToLeft() {
    for (std::size_t i = 0; i < m_Size; i++) {
      new (&m_RawData[i]) T(std::move_if_noexcept(m_RawData[i + 1]));
    }
  }
  template <class F>
  void m_ShiftFromTo(std::size_t from, std::size_t to, F &&func) {
    for (; from < to; from++) {
      new (&m_RawData[from])
          T(std::move_if_noexcept(m_RawData[func(from, to)]));
    }
  }
  template <class It> void m_ShiftRangeFromTo(It from, It to) {
    for (; from != to; from++) {
      new (std::addressof(*from))
          T(std::move_if_noexcept(*(from + (to - from))));
    }
  }
  template <class Iter> void m_DestroyRange(Iter beg, Iter end) {
    for (; beg != end; beg++) {
      m_AllocTraits.destroy(m_Alloc, std::addressof(*beg));
    }
  }
  void m_CheckOrAlloc(std::size_t t_Size) {
    if (t_Size >= m_Capacity) {
      m_ReallocAnyway(m_Capacity * 2);
    }
  }

public:
  using value_type = std::remove_cv_t<T>;
  using allocator_type = Allocator;
  using size_type = decltype(m_Size);
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = con::rnd_iterator<value_type>;
  using const_iterator = con::rnd_iterator<const value_type>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr queue() = default;

  explicit queue(size_type cap = 5, const Allocator &alloc = Allocator{})
      : m_Alloc(alloc), m_Size(0), m_Capacity(cap),
        m_RawData(m_Alloc.allocate(m_Capacity)) {}

  explicit constexpr queue(std::initializer_list<T> init,
                           const Allocator &alloc = Allocator{})
      : queue(init.begin(), init.end()) {}

  explicit constexpr queue(const queue<value_type> &oth)
      : queue(oth.begin(), oth.end()) {}

  explicit constexpr queue(const queue<value_type> &oth, const Allocator &a)
      : queue(oth.begin(), oth.end(), a) {}

  template <std::input_iterator It, std::sentinel_for<It> S>
  constexpr queue(It begin, S end, const Allocator &a = {}) : m_Alloc{a} {
    while (begin != end)
      enqueue(*begin++);
  }
  template <std::forward_iterator It, std::sentinel_for<It> S>
  constexpr queue(It begin, S end, const Allocator &a = {}) : m_Alloc{a} {
    reserve(std::distance(begin, end));
    std::uninitialized_copy(begin, end, m_RawData);
    m_Size = m_Capacity;
  }
  explicit queue(const queue<value_type> &&oth) = delete;
  iterator begin() noexcept { return iterator(m_RawData); }
  iterator end() noexcept { return iterator(m_RawData + size()); }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_iterator begin() const noexcept { return const_iterator(m_RawData); }
  const_iterator end() const noexcept {
    return const_iterator(m_RawData + size());
  }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(m_RawData + size());
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(m_RawData);
  }

  const_iterator cbegin() const noexcept { return const_iterator(m_RawData); }
  const_iterator cend() const noexcept {
    return const_iterator(m_RawData + size());
  }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  bool empty() const noexcept { return size() == 0; }
  size_type size() const noexcept { return m_Size; }
  size_type capacity() const noexcept { return m_Capacity; }
  const_pointer data() const { return m_RawData; }
  reference data() { return m_RawData; }
  void clear() {
    for (size_type i = 0; i < size(); i++) {
      m_AllocTraits.destroy(m_Alloc, std::addressof(m_RawData[i]));
    }
    m_Size = 0;
  }
  void reserve(size_type cp) { m_CheckOrAlloc(cp); }
  void resize(size_type sz) {
    m_Size = sz;
    m_CheckOrAlloc(sz);
  }
  void erase(iterator val) {
    if (val != end()) {
      difference_type x = val - begin();
      pointer p = m_RawData + x;
      m_AllocTraits.destroy(m_Alloc, std::addressof(*val));
      m_ShiftFromTo(std::distance(begin(), iterator(p)), size(),
                    [](auto l, [[maybe_unused]] auto _) { return l + 1; });
      m_Size--;
    } else {
      return;
    }
  }
  void erase(iterator first, iterator last) {
    assert(first <= last && "queue::erase invalid range");
    m_DestroyRange(first, last);
    m_ShiftRangeFromTo(first, last);
    m_Size -= std::distance(first, last);
  }
  void erase(reverse_iterator first, reverse_iterator last) {
    assert(first <= last && "queue::erase invalid range");
    m_DestroyRange(first, last);
    m_ShiftRangeFromTo(first, last);
    m_Size -= std::distance(first, last);
  }
  void erase(reverse_iterator val) {
    if (val != rend()) {
      m_AllocTraits.destroy(m_Alloc, std::addressof(*val));
      m_ShiftFromTo(std::distance(val, rend()) - 1, size(),
                    [](auto l, [[maybe_unused]] auto _) { return l + 1; });
      m_Size--;
    } else {
      return;
    }
  }
  void erase(const value_type &obj) { erase(std::find(begin(), end(), obj)); }
  void rerase(const value_type &obj) {
    erase(std::find(rbegin(), rend(), obj));
  }

  void enqueue(const value_type &oth) requires(
      std::is_copy_constructible<value_type>::value) {
    m_CheckOrAlloc(size());
    new (&m_RawData[m_Size++]) value_type(oth);
  }
  void enqueue(value_type &&oth) requires(
      std::is_move_constructible<value_type>::value) {
    m_CheckOrAlloc(size());
    new (&m_RawData[m_Size++]) value_type(std::move(oth));
  }

  value_type dequeue() requires(std::is_destructible<value_type>::value) {
    --m_Size;
    value_type temp = m_RawData[0];
    m_AllocTraits.destory(m_Alloc, std::addressof(m_RawData[0]));
    m_ShiftToLeft();
    return temp;
  }

  template <class... Args> void emplace(Args &&...args) {
    enqueue(value_type(std::forward<Args>(args)...));
  }

  value_type at(size_type index) const {
    if (index >= size()) {
      throw std::range_error("out of bounds queue");
    } else {
      return m_RawData[index];
    }
  }
  reference at(size_type index) {
    if (index >= size()) {
      throw std::range_error("out of bounds queue");
    } else {
      return m_RawData[index];
    }
  }
  value_type operator[](size_type index) const { return m_RawData[index]; }
  reference operator[](size_type index) { return m_RawData[index]; }

  queue<value_type> &operator=(const queue<value_type> &oth) {
    if (&oth != this) {
      clear();
      m_Size = oth.size();
      m_CheckOrAlloc(m_Size);
      std::uninitialized_copy(oth.begin(), oth.end(), m_RawData);
    }
    return *this;
  }
  queue<value_type> &operator=(queue<value_type> &&oth) {
    if (&oth != this) {
      clear();
      m_Size = oth.size();
      m_CheckOrAlloc(m_Size);
      std::uninitialized_move(oth.begin(), oth.end(), m_RawData);
      oth.~queue();
    }
    return *this;
  }
  ~queue() {
    clear();
    m_Alloc.deallocate(m_RawData, m_Capacity);
    std::exchange(m_RawData, nullptr);
    std::exchange(m_Size, 0);
  }
};
} // namespace con
#endif
