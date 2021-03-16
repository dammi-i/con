#ifndef CON_QUEUE_HPP
#define CON_QUEUE_HPP
#include <algorithm>
#include <cassert>
#include <compare>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace con {
template <class T, class Allocator = std::allocator<T> >
class queue {
  T *m_RawData = nullptr;
  std::size_t m_Head = 0;
  std::size_t m_Size = 0;
  std::size_t m_Capacity = 0;
  [[no_unique_address]] Allocator m_Alloc;

  using alloc_traits = std::allocator_traits<Allocator>;

  void m_ReallocAnyway(std::size_t t_NewCapacity) {
    T *f_temp = alloc_traits::allocate(m_Alloc, t_NewCapacity);
    std::size_t count{0};
    if (m_Size == 0) {
      m_DefaultAlloc();
    } else {
      try {
        for (; count < m_Size; count++) {
          alloc_traits::construct(m_Alloc, f_temp + count,
                                  std::move_if_noexcept(*(m_RawData + count)));
          alloc_traits::destroy(m_Alloc, m_RawData + count);
        }
        alloc_traits::deallocate(m_Alloc, m_RawData, m_Capacity);
        std::exchange(m_RawData, f_temp);
      } catch (...) {
        for (std::size_t i = count; i > 0; i--) {
          alloc_traits::destroy(m_Alloc, f_temp + (i - 1));
        }
        alloc_traits::deallocate(m_Alloc, f_temp, t_NewCapacity);
        throw;
      }

      for (std::size_t i = m_Size; i > 0; i--) {
        alloc_traits::destroy(m_Alloc, m_RawData + (i - 1));
      }

      alloc_traits::deallocate(m_Alloc, m_RawData, m_Capacity);
      m_Capacity = t_NewCapacity;
      m_RawData = f_temp;
    }
  }

  void m_DefaultAlloc(std::size_t x = 4) {
    m_Capacity = x;
    m_RawData = alloc_traits::allocate(m_Alloc, m_Capacity);
  }

  template <class Iter>
  void m_DestroyRange(Iter beg, Iter end) {
    for (; beg != end; beg++) {
      alloc_traits::destroy(m_Alloc, std::addressof(*beg));
    }
  }
  void m_CheckOrAlloc(std::size_t t_Size) {
    if (t_Size >= m_Capacity) {
      m_ReallocAnyway(m_Capacity * 1.5);
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
  struct rnd_iterator {
    pointer m_Ptr;
    /*
     * type aliases
     */
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = difference_type;
    using value_type = std::remove_cv_t<T>;
    /*
     * constructors
     */
    explicit rnd_iterator(pointer p) noexcept : m_Ptr(p) {}
    /*
     * access operators
     */
    reference operator*() { return *m_Ptr; }
    reference operator[](size_type idx) { return m_Ptr[idx]; }
    pointer operator->() { return m_Ptr; }
    /*
     * increment/decrement and assign operators
     */
    rnd_iterator &operator+=(difference_type n) {
      m_Ptr += n;
      return *this;
    }
    rnd_iterator &operator-=(difference_type n) {
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

    constexpr auto operator<=>(const rnd_iterator &rhs) const noexcept =
        default;

    difference_type operator-(const rnd_iterator &it) {
      return (m_Ptr - it.m_Ptr);
    }

    friend rnd_iterator operator+(const rnd_iterator &it, difference_type n) {
      return rnd_iterator(it.m_Ptr + n);
    }
    friend rnd_iterator operator+(difference_type n, const rnd_iterator &it) {
      return rnd_iterator(it.m_Ptr + n);
    }
    friend rnd_iterator operator-(const rnd_iterator &it, difference_type n) {
      return rnd_iterator(it.m_Ptr - n);
    }
    friend rnd_iterator operator-(difference_type n, const rnd_iterator &it) {
      return rnd_iterator(it.m_Ptr - n);
    }
  };
  struct rnd_const_iterator {
    // clang-format off
      const_pointer m_Ptr;
      /*
       * type aliases
       */
    // clang-format on
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = difference_type;
    using value_type = std::remove_cv_t<T>;
    /*
     * constructors
     */
    explicit rnd_const_iterator(pointer p) noexcept : m_Ptr(p) {}
    /*
     * access operators
     */
    const_reference operator*() { return *m_Ptr; }
    const_reference operator[](size_type idx) { return m_Ptr[idx]; }
    const_pointer operator->() { return m_Ptr; }
    /*
     * increment/decrement and assign operators
     */
    rnd_const_iterator &operator+=(difference_type n) {
      m_Ptr += n;
      return *this;
    }
    rnd_const_iterator &operator-=(difference_type n) {
      m_Ptr += n;
      return *this;
    }
    rnd_const_iterator &operator++() {
      ++m_Ptr;
      return *this;
    }
    rnd_const_iterator operator++(int) {
      auto temp = *this;
      m_Ptr++;
      return temp;
    }
    rnd_const_iterator &operator--() {
      --m_Ptr;
      return *this;
    }
    rnd_const_iterator operator--(int) {
      auto temp = *this;
      m_Ptr--;
      return temp;
    }

    constexpr auto operator<=>(const rnd_iterator &rhs) const noexcept =
        default;

    difference_type operator-(const rnd_iterator &it) {
      return (m_Ptr - it.m_Ptr);
    }

    friend rnd_const_iterator operator+(const rnd_iterator &it,
                                        difference_type n) {
      return rnd_const_iterator(it.m_Ptr + n);
    }
    friend rnd_const_iterator operator+(difference_type n,
                                        const rnd_iterator &it) {
      return rnd_const_iterator(it.m_Ptr + n);
    }
    friend rnd_const_iterator operator-(const rnd_iterator &it,
                                        difference_type n) {
      return rnd_const_iterator(it.m_Ptr - n);
    }
    friend rnd_const_iterator operator-(difference_type n,
                                        const rnd_iterator &it) {
      return rnd_const_iterator(it.m_Ptr - n);
    }
  };
  using iterator = rnd_iterator;
  using const_iterator = rnd_const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr queue() noexcept = default;

  constexpr explicit queue(const Allocator &alloc) noexcept : m_Alloc(alloc) {}

  explicit constexpr queue(size_type count, const value_type &vl = {},
                           const Allocator &alloc = {})
      : m_Alloc(alloc), m_Size(count), m_Capacity(count) {
    m_DefaultAlloc(count);
    std::uninitialized_fill_n(m_RawData, count, vl);
  }

  explicit constexpr queue(std::initializer_list<T> init,
                           const Allocator &alloc = Allocator{})
      : queue(init.begin(), init.end(), alloc) {}

  explicit constexpr queue(queue &&q) noexcept : m_Alloc(q.get_allocator()) {
    swap(*this, std::move(q));
  }

  explicit constexpr queue(queue &&q, const Allocator &alloc) noexcept(
      alloc_traits::is_always_equal::value)
      : m_Alloc(alloc) {
    if (alloc_traits::is_always_equal::value || m_Alloc == q.get_allocator()) {
      swap(*this, std::move(q));
    } else {
      m_DefaultAlloc(q.size());
      std::uninitialized_move(q.begin(), q.end(), m_RawData);
      m_Capacity = q.capacity();
      m_Size = q.size();
    }
  }

  explicit constexpr queue(const queue<value_type> &oth)
      : queue(oth.begin(), oth.end()) {}

  explicit constexpr queue(const queue<value_type> &oth, const Allocator &alloc)
      : queue(oth.begin(), oth.end(), alloc) {}

  template <class It>
  constexpr queue(It begin, It end, Allocator a = {})
      : m_RawData(alloc_traits::allocate(a, 4)), m_Capacity(4), m_Alloc(a) {
    while (begin != end) enqueue(*begin++);
  }
  iterator begin() noexcept { return iterator(m_RawData + m_Head); }
  iterator end() noexcept { return iterator(m_RawData + m_Size); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  iterator begin() const noexcept { return iterator(m_RawData + m_Head); }
  iterator end() const noexcept { return iterator(m_RawData + m_Size); }

  reverse_iterator rbegin() const noexcept {
    return reverse_iterator(m_RawData + m_Size);
  }
  reverse_iterator rend() const noexcept {
    return reverse_iterator(m_RawData + m_Head);
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(m_RawData + m_Head);
  }
  const_iterator cend() const noexcept {
    return const_iterator(m_RawData + size());
  }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  constexpr bool empty() const noexcept { return size() == 0; }

  constexpr size_type size() const noexcept { return (m_Size - m_Head); }

  constexpr size_type max_size() const noexcept {
    return (std::numeric_limits<size_type>::max() / sizeof(value_type));
  }
  constexpr allocator_type get_allocator() const noexcept { return m_Alloc; }

  constexpr size_type capacity() const noexcept { return m_Capacity; }

  const_reference data() const { return m_RawData; }

  reference data() { return m_RawData; }

  const_reference front() const { return *begin(); }
  const_reference back() const { return *end(); }

  reference front() { return *begin(); }
  reference back() { return *end(); }

  void clear() noexcept {
    for (size_type i = 0; i < size(); i++) {
      alloc_traits::destroy(m_Alloc, m_RawData + i);
    }
    m_Size = 0;
  }

  void reserve(size_type cp) { m_CheckOrAlloc(cp); }

  void resize(size_type sz) {
    if (sz < size()) {
      m_DestroyRange(begin() + sz, end());
      m_Size = sz;
    } else if (sz > size()) {
      for (size_type i = size(); i < sz; i++) enqueue(value_type{});
    }
  }

  void erase(iterator val) {
    if (val != end()) {
      erase(val, val + 1);
    } else {
      return;
    }
  }
  void erase(iterator first, iterator last) {
    if (last == end()) {
      for (; first != last; first++) {
        alloc_traits::destroy(m_Alloc, std::addressof(*first));
        m_Size--;
      }
    } else {
      std::move(last, end(), first);
      for (; first != last; first++) {
        alloc_traits::destroy(m_Alloc, std::addressof(*first));
        m_Size--;
      }
    }
  }

  void enqueue(const value_type &oth) { emplace(oth); }

  void enqueue(value_type &&oth) { emplace(std::move(oth)); }

  value_type dequeue() {
    value_type temp = m_RawData[0];
    alloc_traits::destroy(m_Alloc, std::addressof(m_RawData[0]));
    m_Head++;
    return temp;
  }

  template <class... Args>
  void emplace(Args &&...args) {
    m_CheckOrAlloc(size());
    alloc_traits::construct(m_Alloc, (m_RawData + m_Size),
                            std::forward<Args>(args)...);
    m_Size++;
  }

  const_reference at(size_type index) const {
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
  const_reference operator[](size_type index) const { return m_RawData[index]; }
  reference operator[](size_type index) { return m_RawData[index]; }

  constexpr queue<value_type> &operator=(queue<value_type> &oth) {
    swap(*this, oth);
    return *this;
  }
  constexpr queue<value_type> &operator=(queue<value_type> &&oth) noexcept(
      alloc_traits::propagate_on_container_move_assignment::value ||
      alloc_traits::is_always_equal::value) {
    if (alloc_traits::propagate_on_container_move_assignment::value) {
      clear();
      m_Alloc = std::move(oth.get_allocator());
      swap(*this, std::move(oth));
    } else if (alloc_traits::is_always_equal::value ||
               m_Alloc == oth.get_allocator()) {
      swap(*this, std::move(oth));
    } else {
      m_CheckOrAlloc(oth.size());
      std::uninitialized_move(oth.begin(), oth.end(), m_RawData + m_Head);
    }
    return *this;
  }

  friend constexpr void swap(queue &lhs, queue &&rhs) noexcept {
    using std::swap;
    swap(lhs.m_RawData, rhs.m_RawData);
    swap(lhs.m_Size, rhs.m_Size);
    swap(lhs.m_Capacity, rhs.m_Capacity);
  }

  ~queue() noexcept {
    clear();
    alloc_traits::deallocate(m_Alloc, m_RawData, m_Capacity);
    std::exchange(m_RawData, nullptr);
    std::exchange(m_Capacity, 0);
    std::exchange(m_Head, 0);
  }
};
}  // namespace con
#endif
