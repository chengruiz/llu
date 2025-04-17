#ifndef LLU_CHRONO_H_
#define LLU_CHRONO_H_

#include <chrono>
#include <thread>

#include <fmt/core.h>

#include <llu/math.h>

namespace llu {
using Clock     = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration  = Clock::duration;

using MSec = std::chrono::milliseconds;
using USec = std::chrono::microseconds;
using NSec = std::chrono::nanoseconds;
using std::chrono::duration_cast;

template <typename Unit>
typename Unit::rep timePassed(const TimePoint &start) {
  return duration_cast<Unit>(Clock::now() - start).count();
}

class Rate {
 public:
  explicit Rate(std::size_t freq) : cycle_(static_cast<std::size_t>(1e9) / freq), event_time_(Clock::now()) {}

  void sleep() {
    event_time_ += cycle_;
    while (Clock::now() < event_time_ - USec(50)) {
      std::this_thread::sleep_for(USec(50));
    }
    while (Clock::now() < event_time_);
  }

 private:
  NSec cycle_;
  TimePoint event_time_;
};

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
    count_    = 0;
  }

  [[nodiscard]] std::size_t count() const { return count_; }
  [[nodiscard]] Duration total() const { return duration_; }

  [[nodiscard]] Duration mean() const {
    if (count_ == 0) return Duration{0};
    return duration_ / count_;
  }

  template <typename T>
  [[nodiscard]] T total() {
    return duration_cast<T>(total());
  }
  template <typename T>
  [[nodiscard]] T mean() {
    return duration_cast<T>(mean());
  }

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

template <typename Unit = USec>
class IntervalStats {
 public:
  explicit IntervalStats() = default;
  std::size_t count() const { return count_; }
  void clear();
  void tick();
  std::string infoStr();

 private:
  using Rep = typename Unit::rep;

  bool first_{true};
  TimePoint last_time_;
  Rep max_{}, min_{};
  double mean_{}, var_{};
  std::size_t count_{};
};

template <typename Unit>
void IntervalStats<Unit>::clear() {
  count_ = 0;
  mean_  = 0.;
  var_   = 0.;
  max_   = 0;
  min_   = std::numeric_limits<Rep>::max();
}

template <typename Unit>
void IntervalStats<Unit>::tick() {
  auto now = Clock::now();
  if (first_) {
    last_time_ = now;
    max_       = 0;
    min_       = std::numeric_limits<Rep>::max();
    first_     = false;
    return;
  }

  auto duration = duration_cast<Unit>(now - last_time_).count();
  last_time_    = now;
  max_          = std::max(max_, duration);
  min_          = std::min(min_, duration);

  double w0 = static_cast<double>(count_) / (count_ + 1);
  double w1 = 1. / (count_ + 1);

  mean_ = mean_ * w0 + static_cast<double>(duration) * w1;
  var_  = var_ * w0 + square(mean_ - static_cast<double>(duration)) * w0 * w1;
  count_ += 1;
}

template <typename Unit>
std::string IntervalStats<Unit>::infoStr() {
  return fmt::format("mean = {}, stddev = {}, max = {}, min = {}.", Unit(static_cast<Rep>(mean_)),
                     Unit(static_cast<Rep>(std::sqrt(var_ + 1e-8))), Unit(max_), Unit(min_));
}
}  // namespace llu

#endif  // LLU_CHRONO_H_
