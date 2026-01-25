#ifndef LLU_RANGE_H_
#define LLU_RANGE_H_

#include <utility>

#include <llu/math.h>

namespace llu {
template <typename T>
struct range_t : public std::pair<T, T> {
  using std::pair<T, T>::pair;
  [[nodiscard]] T &lower() { return std::pair<T, T>::first; }
  [[nodiscard]] const T &lower() const { return std::pair<T, T>::first; }
  [[nodiscard]] T &upper() { return std::pair<T, T>::second; }
  [[nodiscard]] const T &upper() const { return std::pair<T, T>::second; }
  [[nodiscard]] T mid() const { return (lower() + upper()) / 2; }
};

template <typename T>
constexpr T clamp(T val, const range_t<T> &range) {
  return clamp(val, range.lower(), range.upper());
}

template <typename T>
constexpr T lerp(range_t<T> range, T factor) {
  return lerp(range.lower(), range.upper(), factor);
}
}  // namespace llu

#endif  // LLU_RANGE_H_
