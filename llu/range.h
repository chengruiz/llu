#ifndef LLU_RANGE_H_
#define LLU_RANGE_H_

#include <utility>

#include <llu/math.h>

namespace llu {
template<typename T>
class range_t : public std::pair<T, T> {
  using Pair = std::pair<T, T>;

public:
  using std::pair<T, T>::pair;
  [[nodiscard]] T &lower() { return Pair::first; }
  [[nodiscard]] const T &lower() const { return Pair::first; }
  [[nodiscard]] T &upper() { return Pair::second; }
  [[nodiscard]] const T &upper() const { return Pair::second; }
  [[nodiscard]] T mid() const { return (lower() + upper()) / 2; }
};

using std::clamp;

template<typename T>
T clamp(T val, range_t<T> range) {
  return std::clamp(val, range.lower(), range.upper());
}

template<typename T>
T interpolate(range_t<T> range, T k) { return interpolate(range.lower(), range.upper(), k); }
} // namespace llu


#endif // LLU_RANGE_H_
