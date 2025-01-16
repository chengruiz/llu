#ifndef LLU_SQUEUE_H_
#define LLU_SQUEUE_H_

#include <cstdint>
#include <exception>
#include <vector>

// @formatter:off
namespace llu {
template<typename T>
class StaticQueue {
public:
  StaticQueue() = default;
  explicit StaticQueue(std::size_t capacity) : data_(capacity), capacity_(capacity) {}
  void allocate(std::size_t size);

  [[nodiscard]] bool is_empty() const noexcept { return size_ == 0; }
  [[nodiscard]] bool is_full() const noexcept { return size_ == capacity_; }
  [[nodiscard]] std::size_t size() const noexcept { return size_; }

  void push_back(T      &&item) noexcept { emplace_back(std::move(item)); }
  void push_back(const T &item) noexcept { emplace_back(item); }
  template<typename...Args> void emplace_back(Args &&...args);

  [[nodiscard]] T       &front()       { assert_not_empty(); return data_[front_]; }
  [[nodiscard]] const T &front() const { assert_not_empty(); return data_[front_]; }
  [[nodiscard]] T       &back()        { assert_not_empty(); return data_[(front_ + size_ - 1) % capacity_]; }
  [[nodiscard]] const T &back()  const { assert_not_empty(); return data_[(front_ + size_ - 1) % capacity_]; }
  [[nodiscard]] T       &at(int64_t idx)       { return data_[get_index(idx)]; }
  [[nodiscard]] const T &at(int64_t idx) const { return data_[get_index(idx)]; }
  [[nodiscard]] const T &get(int64_t idx, const T &default_value) const;
  [[nodiscard]] const T &get_padded(int64_t idx) const;

  void clear() noexcept { front_ = size_ = 0; }
  void clear_all() noexcept { clear(); data_.clear(); }

  struct iterator;
  struct const_iterator;
  iterator       begin()       { return iterator(this);              }
  const_iterator begin() const { return const_iterator(this);        }
  iterator       end()         { return iterator(this, size_);       }
  const_iterator end()   const { return const_iterator(this, size_); }

  struct IndexOutOfRange : std::exception { const char *what() const noexcept override { return "Queue Index Out Of Range!"; }  };
  struct EmptyQueue      : std::exception { const char *what() const noexcept override { return "Empty Queue!"; } };

private:
  void assert_not_empty() const { if (is_empty()) throw EmptyQueue(); }
  [[nodiscard]] int64_t get_index(int64_t idx) const;

  std::vector<T> data_{};
  std::size_t front_ = 0, size_ = 0;
  std::size_t capacity_ = 0;
};

template<typename T> void StaticQueue<T>::allocate(std::size_t size) {
  if (data_.empty()) {
    data_.resize(size);
    capacity_ = size;
  } else {
    std::vector<T> temp(size);
    std::size_t start = 0, copy_num = std::min(size, size_);
    if (size_ > size) start = size_ - size;
    for (std::size_t i{}; i < copy_num; ++i) {
      temp[i] = at(start + i);
    }
    if (size_ > size) size_ = size;
    front_ = 0;
    capacity_ = size;
    data_ = std::move(temp);
  }
}

template<typename T> template<typename ...Args> void StaticQueue<T>::emplace_back(Args &&...args) {
  if (is_full()) {
    front_ = (front_ + 1) % capacity_;
  } else {
    ++size_;
  }
  data_[(front_ + size_ - 1) % capacity_] = T(std::forward<Args>(args)...);
}

template<typename T> const T &StaticQueue<T>::get(int64_t idx, const T &default_value) const {
  if (idx < 0) idx += size_;
  if (idx < 0 or idx >= size_) return default_value;
  return data_[(front_ + idx) % capacity_];
}

template<typename T> const T &StaticQueue<T>::get_padded(int64_t idx) const {
  if (idx < 0) idx = static_cast<int64_t>(size_) + idx;
  if (idx < 0) return front();
  if (idx >= size_) return back();
  return data_[(front_ + idx) % capacity_];
}

template<typename T> int64_t StaticQueue<T>::get_index(int64_t idx) const {
  if (idx < 0) idx += size_;
  if (idx < 0 or idx >= size_) throw IndexOutOfRange();
  return (front_ + idx) % capacity_;
}

template<typename T>
struct StaticQueue<T>::iterator {
  explicit iterator(StaticQueue *q, std::size_t idx = 0) : q_(q), idx_(idx) {}
  iterator &operator++() { idx_++; return *this; }
  bool operator==(iterator other) const { return idx_ == other.idx_ && q_ == other.q_; }
  bool operator!=(iterator other) const { return idx_ != other.idx_ || q_ != other.q_; }
  T &operator*() const { return q_->at(idx_); }

private:
  StaticQueue *q_;
  std::size_t idx_;
};

template<typename T>
struct StaticQueue<T>::const_iterator {
  explicit const_iterator(const StaticQueue *q, std::size_t idx = 0) : q_(q), idx_(idx) {}
  const_iterator &operator++() { idx_++; return *this; }
  bool operator==(const_iterator other) const { return idx_ == other.idx_ && q_ == other.q_; }
  bool operator!=(const_iterator other) const { return idx_ != other.idx_ || q_ != other.q_; }
  const T &operator*() const { return q_->at(idx_); }

private:
  const StaticQueue *q_;
  std::size_t idx_;
};

} // namespace llu


#endif  // LLU_SQUEUE_H_
