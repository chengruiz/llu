#ifndef LLU_CHRONO_H_
#define LLU_CHRONO_H_

#include <chrono>

namespace llu {
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;
using MSec = std::chrono::milliseconds;
using USec = std::chrono::microseconds;
using NSec = std::chrono::nanoseconds;
using std::chrono::duration_cast;

template<typename Unit>
typename Unit::rep timePassed(const TimePoint &start) {
  return duration_cast<Unit>(Clock::now() - start).count();
}

class Timer {
public:
  void start() { start_ = Clock::now(); }

  void stop() {
    stop_ = Clock::now();
    count_ += 1;
    duration_ += stop_ - start_;
  }

  void clear() {
    duration_ = Duration::zero();
    count_ = 0;
  }

  [[nodiscard]] std::size_t count() const { return count_; }
  [[nodiscard]] Duration total() const { return duration_; }

  [[nodiscard]] Duration mean() const {
    if (count_ == 0) return Duration{0};
    return duration_ / count_;
  }

  template<typename T> [[nodiscard]] T total() { return duration_cast<T>(total()); }
  template<typename T> [[nodiscard]] T mean() { return duration_cast<T>(mean()); }

private:
  TimePoint start_, stop_;
  Duration duration_{0};
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
