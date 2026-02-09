#ifndef LLU_RING_H_
#define LLU_RING_H_

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <utility>
#include <vector>

#include <llu/exception.h>

namespace llu {
template <typename T>
class RingBuffer {
 public:
  RingBuffer() = default;
  explicit RingBuffer(std::size_t capacity) : data_(capacity), capacity_(capacity) {}
  void allocate(std::size_t size);
  void fill(const T &value);

  [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }
  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
  [[nodiscard]] bool full() const noexcept { return size_ == capacity_; }
  [[nodiscard]] std::size_t size() const noexcept { return size_; }

  // clang-format off
  void push_back(T      &&item) { emplace_back(std::move(item)); }
  void push_back(const T &item) { emplace_back(item); }
  template<typename...Args> void emplace_back(Args &&...args);

  void push_front(T      &&item) { emplace_front(std::move(item)); }
  void push_front(const T &item) { emplace_front(item); }
  template<typename...Args> void emplace_front(Args &&...args);

  void pop_front() { assertNotEmpty("pop_front"); front_ = (front_ + 1) % capacity_; --size_; }
  void pop_back() { assertNotEmpty("pop_back"); --size_; }

  [[nodiscard]] T       &front()       { assertNotEmpty("front"); return data_[front_]; }
  [[nodiscard]] const T &front() const { assertNotEmpty("front"); return data_[front_]; }
  [[nodiscard]] T       &back()        { assertNotEmpty("back"); return data_[(front_ + size_ - 1) % capacity_]; }
  [[nodiscard]] const T &back()  const { assertNotEmpty("back"); return data_[(front_ + size_ - 1) % capacity_]; }
  [[nodiscard]] T       &at(int64_t index)       { return data_[resolveIndex(index)]; }
  [[nodiscard]] const T &at(int64_t index) const { return data_[resolveIndex(index)]; }
  [[nodiscard]] T        at(int64_t index, const T &default_value) const;
  [[nodiscard]] T       &operator[](std::size_t index)       { return data_[(front_ + index) % capacity_]; }
  [[nodiscard]] const T &operator[](std::size_t index) const { return data_[(front_ + index) % capacity_]; }

  void clear() noexcept { front_ = size_ = 0; }
  void reset() noexcept { clear(); data_.clear(); capacity_ = 0; }
  // clang-format on

  struct iterator;
  struct const_iterator;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  // clang-format off
  iterator       begin()        { return iterator(this);              }
  const_iterator begin() const  { return const_iterator(this);        }
  iterator       end()          { return iterator(this, size_);       }
  const_iterator end()   const  { return const_iterator(this, size_); }
  const_iterator cbegin() const { return const_iterator(this);        }
  const_iterator cend()   const { return const_iterator(this, size_); }

  reverse_iterator       rbegin()        { return reverse_iterator(end());         }
  const_reverse_iterator rbegin() const  { return const_reverse_iterator(end());   }
  reverse_iterator       rend()          { return reverse_iterator(begin());       }
  const_reverse_iterator rend()   const  { return const_reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(cend());  }
  const_reverse_iterator crend()   const { return const_reverse_iterator(cbegin());}
  // clang-format on

 private:
  void assertAllocated(const char *operation) const {
    if (capacity_ == 0) throw NotAllocatedError("RingBuffer should be allocated before operation '{}'.", operation);
  }
  void assertNotEmpty(const char *operation) const {
    if (empty()) throw UnderflowError("RingBuffer should not be empty before operation '{}'.", operation);
  }
  [[nodiscard]] int64_t resolveIndex(int64_t index) const;

  std::vector<T> data_{};
  std::size_t front_ = 0, size_ = 0;
  std::size_t capacity_ = 0;
};

template <typename T>
void RingBuffer<T>::allocate(std::size_t size) {
  if (data_.empty()) {
    data_.resize(size);
    capacity_ = size;
    return;
  }
  // Keeps the most recent `size` items and discard the rest
  std::vector<T> tmp(size);
  std::size_t start{};
  std::size_t num_remains = std::min(size, size_);
  if (size_ > size) start = size_ - size;
  for (std::size_t i{}; i < num_remains; ++i) {
    tmp[i] = std::move(at(start + i));
  }
  if (size_ > size) size_ = size;
  front_    = 0;
  capacity_ = size;
  data_     = std::move(tmp);
}

template <typename T>
void RingBuffer<T>::fill(const T &value) {
  assertAllocated("fill");
  std::fill(data_.begin(), data_.end(), value);
  front_ = 0;
  size_  = capacity_;
}

template <typename T>
template <typename... Args>
void RingBuffer<T>::emplace_back(Args &&...args) {
  assertAllocated("emplace_back");
  if (full()) {
    front_ = (front_ + 1) % capacity_;
  } else {
    ++size_;
  }
  data_[(front_ + size_ - 1) % capacity_] = T(std::forward<Args>(args)...);
}

template <typename T>
template <typename... Args>
void RingBuffer<T>::emplace_front(Args &&...args) {
  assertAllocated("emplace_front");
  front_ = (front_ + capacity_ - 1) % capacity_;
  if (not full()) ++size_;
  data_[front_] = T(std::forward<Args>(args)...);
}

template <typename T>
T RingBuffer<T>::at(int64_t index, const T &default_value) const {
  if (index < 0) index += size_;
  if (index < 0 or static_cast<std::size_t>(index) >= size_) return default_value;
  return data_[(front_ + index) % capacity_];
}

template <typename T>
int64_t RingBuffer<T>::resolveIndex(int64_t index) const {
  std::int64_t size_i64 = static_cast<std::int64_t>(size_);
  if (index < -size_i64 or index >= size_i64) {
    throw IndexError("RingBuffer index {} is out of valid range [{}, {}) for size {}.", index, -size_i64, size_i64,
                     size_);
  }
  return (front_ + (index < 0 ? index + size_i64 : index)) % capacity_;
}

template <typename T>
struct RingBuffer<T>::iterator {
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = T;
  using difference_type   = std::ptrdiff_t;
  using pointer           = T *;
  using reference         = T &;

  explicit iterator(RingBuffer *rb, std::size_t index = 0) : rb_(rb), index_(index) {}

  // clang-format off
  iterator &operator++() { ++index_; return *this; }
  iterator operator++(int) { iterator tmp = *this; ++index_; return tmp; }
  iterator &operator--() { --index_; return *this; }
  iterator operator--(int) { iterator tmp = *this; --index_; return tmp; }
  iterator &operator+=(difference_type n) { index_ += n; return *this; }
  iterator &operator-=(difference_type n) { index_ -= n; return *this; }
  // clang-format on

  friend iterator operator+(iterator it, difference_type n) { return it += n; }
  friend iterator operator+(difference_type n, iterator it) { return it += n; }
  friend iterator operator-(iterator it, difference_type n) { return it -= n; }
  friend difference_type operator-(const iterator &lhs, const iterator &rhs) {
    return static_cast<difference_type>(lhs.index_) - static_cast<difference_type>(rhs.index_);
  }

  friend bool operator==(const iterator &lhs, const iterator &rhs) {
    return lhs.index_ == rhs.index_ && lhs.rb_ == rhs.rb_;
  }
  friend bool operator!=(const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }
  friend bool operator<(const iterator &lhs, const iterator &rhs) { return lhs.index_ < rhs.index_; }
  friend bool operator>(const iterator &lhs, const iterator &rhs) { return lhs.index_ > rhs.index_; }
  friend bool operator<=(const iterator &lhs, const iterator &rhs) { return lhs.index_ <= rhs.index_; }
  friend bool operator>=(const iterator &lhs, const iterator &rhs) { return lhs.index_ >= rhs.index_; }

  T &operator*() const { return (*rb_)[index_]; }
  T *operator->() const { return &(*rb_)[index_]; }
  T &operator[](difference_type n) const { return (*rb_)[index_ + n]; }

  friend struct const_iterator;

 private:
  RingBuffer *rb_;
  std::size_t index_;
};

template <typename T>
struct RingBuffer<T>::const_iterator {
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = T;
  using difference_type   = std::ptrdiff_t;
  using pointer           = const T *;
  using reference         = const T &;

  explicit const_iterator(const RingBuffer *rb, std::size_t index = 0) : rb_(rb), index_(index) {}
  const_iterator(const iterator &other) : rb_(other.rb_), index_(other.index_) {}

  // clang-format off
  const_iterator &operator++() { ++index_; return *this; }
  const_iterator operator++(int) { const_iterator tmp = *this; ++index_; return tmp; }
  const_iterator &operator--() { --index_; return *this; }
  const_iterator operator--(int) { const_iterator tmp = *this; --index_; return tmp; }
  const_iterator &operator+=(difference_type n) { index_ += n; return *this; }
  const_iterator &operator-=(difference_type n) { index_ -= n; return *this; }
  friend const_iterator operator+(const_iterator it, difference_type n) { return it += n; }
  friend const_iterator operator+(difference_type n, const_iterator it) { return it += n; }
  friend const_iterator operator-(const_iterator it, difference_type n) { return it -= n; }
  friend difference_type operator-(const const_iterator &lhs, const const_iterator &rhs) {
    return static_cast<difference_type>(lhs.index_) - static_cast<difference_type>(rhs.index_);
  }
  // clang-format on

  friend bool operator==(const const_iterator &lhs, const const_iterator &rhs) {
    return lhs.index_ == rhs.index_ && lhs.rb_ == rhs.rb_;
  }
  friend bool operator!=(const const_iterator &lhs, const const_iterator &rhs) { return !(lhs == rhs); }
  friend bool operator<(const const_iterator &lhs, const const_iterator &rhs) { return lhs.index_ < rhs.index_; }
  friend bool operator>(const const_iterator &lhs, const const_iterator &rhs) { return lhs.index_ > rhs.index_; }
  friend bool operator<=(const const_iterator &lhs, const const_iterator &rhs) { return lhs.index_ <= rhs.index_; }
  friend bool operator>=(const const_iterator &lhs, const const_iterator &rhs) { return lhs.index_ >= rhs.index_; }

  const T &operator*() const { return (*rb_)[index_]; }
  const T *operator->() const { return &(*rb_)[index_]; }
  const T &operator[](difference_type n) const { return (*rb_)[index_ + n]; }

 private:
  const RingBuffer *rb_;
  std::size_t index_;
};
}  // namespace llu

#endif  // LLU_RING_H_
