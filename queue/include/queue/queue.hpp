#pragma once
#include "iterator.hpp"
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace con {
template <class T, class Allocator = std::allocator<T>> class queue {
  std::mutex m_Mutex;
  Allocator m_Alloc;
  T *m_RawData;
  std::size_t m_Size, m_Capacity;
  void m_Realloc(std::size_t t_NewCapacity) {
    if (t_NewCapacity > m_Capacity) {
      std::size_t f_old = m_Capacity;
      T *f_temp = m_Alloc.allocate(sizeof(T) * t_NewCapacity);
      try {
        for (std::size_t i = 0; i < m_Size; i++) {
          new (&f_temp[i]) T(m_RawData[i]);
          m_RawData[i].~T();
        }
        m_Alloc.deallocate(m_RawData, f_old);
        m_RawData = f_temp;
      } catch (const std::exception &exc) {
        m_Alloc.deallocate(f_temp, sizeof(T) * t_NewCapacity);
        throw std::move(exc);
      }
    } else {
      return;
    }
  }
  void m_CheckOrAlloc(std::size_t t_Size) {
    if (t_Size >= m_Capacity) {
      m_Realloc(m_Capacity * 2);
    }
  }

public:
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const reference;
  using pointer = T *;
  using const_pointer = const pointer;
  using iterator = con::rnd_iterator<T>;
  using const_iterator = const iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  explicit queue(size_type cap = (sizeof(value_type) * 50),
                 const Allocator &alloc = Allocator{}) noexcept
      : m_Alloc(alloc), m_Size(0), m_Capacity(cap) {}
  explicit queue(const std::initializer_list<T> &init,
                 const Allocator &alloc = Allocator{}) noexcept
      : queue() {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_CheckOrRealloc(init.size());
    std::copy(init.begin(), init.end(), m_RawData);
  }
  explicit queue(const queue<value_type> &oth) {
    m_Size = oth.size();
    m_Capacity = oth.size();
  }

  iterator begin() noexcept { return iterator(m_RawData[0]); }
  iterator end() noexcept { return iterator(m_RawData[m_Size]); }
  reverse_iterator rbegin() noexcept { return reverse_iterator(m_RawData[0]); }
  reverse_iterator rend() noexcept {
    return reverse_iterator(m_RawData[m_Size]);
  }

  const_iterator begin() const noexcept { return const_iterator(m_RawData[0]); }
  const_iterator end() const noexcept {
    return const_iterator(m_RawData[m_Size]);
  }
  const_reverse_iterator rbegin() const noexcept {
    return reverse_iterator(m_RawData[0]);
  }
  const_reverse_iterator rend() const noexcept {
    return reverse_iterator(m_RawData[m_Size]);
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(m_RawData[0]);
  }
  const_iterator cend() const noexcept {
    return const_iterator(m_RawData[m_Size]);
  }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { rend(); }

  bool empty() const noexcept { return size() == 0; }
  size_type size() const noexcept { return m_Size; }
  size_type capacity() const noexcept { return m_Capacity; }
  size_type max_capacity() const noexcept {
    return std::numeric_limits<size_type>::max();
  }
  const_pointer data() const { return m_RawData; }
  void clear() requires(std::is_destructible<value_type>) {
    for (size_type i = 0; i < size(); i++) {
      m_RawData[i].~value_type();
    }
  }

  void enqueue(const_reference oth) {
    std::lock_guard<std::mutex> lg(m_Mutex);
    m_CheckOrAlloc(size());
    new (&m_RawData[m_Size++]) value_type(oth);
  }
  void enqueue(T &&oth) requires(std::is_move_constructible<value_type>) {
    std::lock_guard<std::mutex> lg(m_Mutex);
    m_CheckOrAllloc(size());
    m_RawData[m_Size] = std::move(oth);
  }
  [[nodiscard]] value_type
  dequeue() requires(std::is_destructible<value_type>) {
    std::lock_guard<std::mutex> lg(m_Mutex);
    --m_Size;
    value_type temp = m_RawData[i];
    m_RawData[i].~value_type();
    return temp;
  }
  ~queue() {
    m_Alloc.deallocate(m_RawData, m_Capacity);
    std::exchange(m_RawData, m_Capacity);
  }
  ~queue() requires(std::is_destructible<value_type>) {
    clear();
    m_Alloc.deallocate(m_RawData, m_Capacity);
    std::exchange(m_RawData, nullptr);
  }
};
} // namespace con
