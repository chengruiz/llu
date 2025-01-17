#ifndef LLU_CHRONO_H_
#define LLU_CHRONO_H_

#include <chrono>

namespace llu {
using clock_t = std::chrono::high_resolution_clock;
using time_point_t = clock_t::time_point;
using duration_t = clock_t::duration;
using ms_t = std::chrono::milliseconds;
using us_t = std::chrono::microseconds;
using ns_t = std::chrono::nanoseconds;
using std::chrono::duration_cast;

template<typename Unit>
typename Unit::rep timePassed(const time_point_t &start) {
  return duration_cast<Unit>(clock_t::now() - start).count();
}

class Timer {
public:
  void start() { start_ = clock_t::now(); }

  void stop() {
    stop_ = clock_t::now();
    count_ += 1;
    duration_ += stop_ - start_;
  }

  void clear() {
    duration_ = duration_t::zero();
    count_ = 0;
  }

  [[nodiscard]] std::size_t count() const { return count_; }
  [[nodiscard]] duration_t total() const { return duration_; }

  [[nodiscard]] duration_t mean() const {
    if (count_ == 0) return duration_t{0};
    return duration_ / count_;
  }

  template<typename T> [[nodiscard]] T total() { return duration_cast<T>(total()); }
  template<typename T> [[nodiscard]] T mean() { return duration_cast<T>(mean()); }

private:
  time_point_t start_, stop_;
  duration_t duration_{0};
  std::size_t count_ = 0;
};

class TimerContext {
public:
  explicit TimerContext(Timer &timer) : timer_{timer} { timer_.start(); }
  ~TimerContext() { timer_.stop(); }

private:
  Timer &timer_;
};
} // namespace llu

#endif  // LLU_CHRONO_H_
