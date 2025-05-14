#ifndef LLU_MATH_H_
#define LLU_MATH_H_

#include <cmath>

#include <llu/const.h>
#include <llu/eigen.h>
#include <llu/macro.h>

#ifndef M_PIf
constexpr float M_PIf = static_cast<float>(M_PI);
#endif  // M_PIf

namespace llu {
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

#if __cplusplus >= 201703L
using std::clamp;
#else
template <typename T>
T clamp(T val, T lower, T upper) {
  return std::max(lower, std::min(val, upper));
}
#endif

template <typename T>
constexpr T lerp(T start, T end, T factor) {
  LLU_ASSERT_FP(T);
  factor = clamp<T>(factor, static_cast<T>(0), static_cast<T>(1));
  return start + (end - start) * factor;
}

template <typename T>
constexpr T ceilDiv(T x, T y) {
  LLU_ASSERT_INT(T);
  return (x + y - 1) / y;
}

template <typename T>
constexpr T pMod(T dividend, T divisor) {  // positive mod, divisor must be positive
  LLU_ASSERT_INT(T);
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

inline ArrXf sigmoid(cArrXf val) { return 1 / (1 + (-val).exp()); }
inline ArrXd sigmoid(cArrXd val) { return 1 / (1 + (-val).exp()); }
}  // namespace llu

#endif  // LLU_MATH_H_
