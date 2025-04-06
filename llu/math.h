#ifndef LLU_MATH_H_
#define LLU_MATH_H_

#include <Eigen/Core>
#include <cmath>

#include <llu/const.h>
#include <llu/macro.h>

namespace llu {
#ifndef M_PIf
constexpr float M_PIf = static_cast<float>(M_PI);
#endif  // M_PIf

template <typename T>
constexpr T square(T x) {
  return x * x;
}

template <typename T>
constexpr T median(T a, T b, T c) {
  return std::max(std::min(a, b), std::min(std::max(a, b), c));
}

template <typename T>
constexpr T deg2rad(T deg) {
  LLU_ASSERT_FP(T);
  return deg * static_cast<T>(M_PI / 180.);
}

template <typename T>
constexpr T rad2deg(T rad) {
  LLU_ASSERT_FP(T);
  return rad * static_cast<T>(180. / M_PI);
}

constexpr float unit(float x) { return x / (std::abs(x) + static_cast<float>(kEPS)); }
constexpr double unit(double x) { return x / (std::abs(x) + kEPS); }

template <typename Derived>
auto unit(const Eigen::MatrixBase<Derived> &x) {
  return x / (x.norm() + kEPS);
}

template <typename T>
constexpr T interpolate(T x0, T x1, T k) {
  LLU_ASSERT_FP(T);
  return x0 + (x1 - x0) * k;
}

template <typename T>
constexpr T ceilDiv(T x, T y) {
  static_assert(std::is_integral<T>());
  return (x + y - 1) / y;
}

template <typename T>
constexpr T pMod(T dividend, T divisor) {  // positive mod, divisor must be positive
  static_assert(std::is_integral<T>());
  return (dividend % divisor + divisor) % divisor;
}

template <typename T>
constexpr T angleDiff(T a1, T a2) {
  LLU_ASSERT_FP(T);
  constexpr T pi = static_cast<T>(M_PI);
  a1 -= static_cast<int>(a1 / (2 * pi)) * 2 * pi;
  a2 -= static_cast<int>(a2 / (2 * pi)) * 2 * pi;
  T diff = a1 - a2;
  if (diff > pi) {
    diff -= 2 * pi;
  } else if (diff < -pi) {
    diff += 2 * pi;
  }
  return diff;
}
}  // namespace llu

#endif  // LLU_MATH_H_
