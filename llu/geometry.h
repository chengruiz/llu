#ifndef LLU_GEOMETRY_H_
#define LLU_GEOMETRY_H_

#include <array>
#include <cmath>
#include <cstdint>

#include <Eigen/Dense>

#include <llu/const.h>
#include <llu/eigen.h>
#include <llu/error.h>
#include <llu/macro.h>
#include <llu/math.h>

namespace llu {
enum class Rotation6dOrder : std::uint8_t { kRowMajor, kColumnMajor };

template <typename T>
class Quaternion {
  LLU_EIGEN_ALIAS(Vec3t, Eigen::Matrix<T, 3, 1>);
  LLU_EIGEN_ALIAS(Vec4t, Eigen::Matrix<T, 4, 1>);
  LLU_EIGEN_ALIAS(Vec6t, Eigen::Matrix<T, 6, 1>);
  LLU_EIGEN_ALIAS(Mat3t, Eigen::Matrix<T, 3, 3>);
  static constexpr T PI = static_cast<T>(M_PI);

 public:
  LLU_ASSERT_FLOATING_POINT(T);
  Quaternion() : data_{1., 0., 0., 0.} {}
  Quaternion(const Quaternion &q) : data_(q.data_) {}
  Quaternion(T w, T x, T y, T z) : data_{w, x, y, z} { normalize(); }
  explicit Quaternion(const std::array<T, 4> &data) : data_(data) { normalize(); }
  explicit Quaternion(cVec4t q) : Quaternion{q[0], q[1], q[2], q[3]} {}
  explicit Quaternion(const Eigen::Quaternion<T> &q) : Quaternion(q.w(), q.x(), q.y(), q.z()) {}
  Quaternion &operator=(const Quaternion &other) = default;
  Quaternion &operator=(const Eigen::Quaternion<T> &q) { return operator=(Quaternion(q.w(), q.x(), q.y(), q.z())); }
  static Quaternion fromMatrix(cMat3t mat);
  static Quaternion fromRoll(T roll) { return {std::cos(roll / 2), std::sin(roll / 2), 0., 0.}; }
  static Quaternion fromPitch(T pitch) { return {std::cos(pitch / 2), 0., std::sin(pitch / 2), 0.}; }
  static Quaternion fromYaw(T yaw) { return {std::cos(yaw / 2), 0., 0., std::sin(yaw / 2)}; }
  static Quaternion fromEulerAngles(const Vec3t &rpy) { return fromEulerAngles(rpy.x(), rpy.y(), rpy.z()); }
  static Quaternion fromEulerAngles(const std::array<T, 3> &rpy) { return fromEulerAngles(rpy[0], rpy[1], rpy[2]); }
  static Quaternion fromEulerAngles(T roll, T pitch, T yaw) { return fromYaw(yaw) * fromPitch(pitch) * fromRoll(roll); }

  void setIdentity() { data_ = {1., 0., 0., 0.}; }
  [[nodiscard]] constexpr T w() const { return data_[0]; }
  [[nodiscard]] constexpr T x() const { return data_[1]; }
  [[nodiscard]] constexpr T y() const { return data_[2]; }
  [[nodiscard]] constexpr T z() const { return data_[3]; }
  [[nodiscard]] Vec4t coeffs() const { return {w(), x(), y(), z()}; }
  [[nodiscard]] Quaternion inverse() const { return {w(), -x(), -y(), -z()}; }
  [[nodiscard]] Mat3t matrix() const;
  [[nodiscard]] Vec3t eulerAngles() const;
  [[nodiscard]] Vec3t rotationVector() const;
  [[nodiscard]] Vec6t rotation6d(Rotation6dOrder order = Rotation6dOrder::kColumnMajor) const;

  [[nodiscard]] Quaternion operator*(const Quaternion &other) const;
  [[nodiscard]] Vec3t operator*(const Vec3t &vec) const;
  [[nodiscard]] Quaternion slerp(const Quaternion &other, T t) const;
  [[nodiscard]] bool isApprox(const Quaternion &other, T tol) const;

 private:
  std::array<T, 4> data_;

  [[nodiscard]] T norm() const { return std::sqrt(squaredNorm()); }
  [[nodiscard]] T squaredNorm() const { return w() * w() + x() * x() + y() * y() + z() * z(); }
  Quaternion &normalize() { return operator/=(norm()); }

  Quaternion operator*(T coef) const { return {w() * coef, x() * coef, y() * coef, z() * coef}; }
  Quaternion &operator*=(T coef);
  Quaternion operator/(T coef) const { return operator*(1 / coef); }
  Quaternion &operator/=(T coef) { return operator*=(1 / coef); }
};

using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;

template <typename T>
auto Quaternion<T>::fromMatrix(cMat3t mat) -> Quaternion {
  // https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
  T trace = mat.trace();
  if (trace > 0) {
    T s = std::sqrt(trace + 1) * 2;
    return {
        static_cast<T>(0.25) * s,
        (mat(2, 1) - mat(1, 2)) / s,
        (mat(0, 2) - mat(2, 0)) / s,
        (mat(1, 0) - mat(0, 1)) / s,
    };
  }

  if (mat(0, 0) > mat(1, 1) and mat(0, 0) > mat(2, 2)) {
    T s = std::sqrt(1 + mat(0, 0) - mat(1, 1) - mat(2, 2)) * 2;
    return {
        (mat(2, 1) - mat(1, 2)) / s,
        static_cast<T>(0.25) * s,
        (mat(0, 1) + mat(1, 0)) / s,
        (mat(0, 2) + mat(2, 0)) / s,
    };
  }

  if (mat(1, 1) > mat(2, 2)) {
    T s = std::sqrt(1 + mat(1, 1) - mat(0, 0) - mat(2, 2)) * 2;
    return {
        (mat(0, 2) - mat(2, 0)) / s,
        (mat(0, 1) + mat(1, 0)) / s,
        static_cast<T>(0.25) * s,
        (mat(1, 2) + mat(2, 1)) / s,
    };
  }

  T s = std::sqrt(1 + mat(2, 2) - mat(0, 0) - mat(1, 1)) * 2;
  return {
      (mat(1, 0) - mat(0, 1)) / s,
      (mat(0, 2) + mat(2, 0)) / s,
      (mat(1, 2) + mat(2, 1)) / s,
      static_cast<T>(0.25) * s,
  };
}

template <typename T>
auto Quaternion<T>::eulerAngles() const -> Vec3t {
  return {
      std::atan2(2 * (w() * x() + y() * z()), 1 - 2 * (x() * x() + y() * y())),
      -PI / 2 + 2 * std::atan2(std::sqrt(1 + 2 * (w() * y() - x() * z())), std::sqrt(1 - 2 * (w() * y() - x() * z()))),
      std::atan2(2 * (w() * z() + x() * y()), 1 - 2 * (y() * y() + z() * z())),
  };
}

template <typename T>
auto Quaternion<T>::matrix() const -> Mat3t {
  Mat3t R;
  R(0, 0) = 1 - 2 * (y() * y() + z() * z());
  R(0, 1) = 2 * (x() * y() - w() * z());
  R(0, 2) = 2 * (x() * z() + w() * y());
  R(1, 0) = 2 * (x() * y() + w() * z());
  R(1, 1) = 1 - 2 * (x() * x() + z() * z());
  R(1, 2) = 2 * (y() * z() - w() * x());
  R(2, 0) = 2 * (x() * z() - w() * y());
  R(2, 1) = 2 * (y() * z() + w() * x());
  R(2, 2) = 1 - 2 * (x() * x() + y() * y());
  return R;
}

template <typename T>
auto Quaternion<T>::rotationVector() const -> Vec3t {
  Vec4t canonical = coeffs();
  if (canonical[0] < 0) canonical = -canonical;

  Vec3t imag       = canonical.template tail<3>();
  const T sin_half = imag.norm();
  if (sin_half < kEPS) return static_cast<T>(2) * imag;
  return imag * (static_cast<T>(2) * std::atan2(sin_half, canonical[0]) / sin_half);
}

template <typename T>
auto Quaternion<T>::rotation6d(Rotation6dOrder order) const -> Vec6t {
  Vec6t out;
  const auto m = matrix();
  switch (order) {
    case Rotation6dOrder::kRowMajor:
      out << m(0, 0), m(0, 1), m(1, 0), m(1, 1), m(2, 0), m(2, 1);
      break;
    case Rotation6dOrder::kColumnMajor:
      out << m(0, 0), m(1, 0), m(2, 0), m(0, 1), m(1, 1), m(2, 1);
      break;
    default:
      LLU_UNREACHABLE();
  }
  return out;
}

template <typename T>
auto Quaternion<T>::operator*=(T coef) -> Quaternion & {
  data_[0] *= coef;
  data_[1] *= coef;
  data_[2] *= coef;
  data_[3] *= coef;
  return *this;
}

template <typename T>
auto Quaternion<T>::operator*(const Quaternion &other) const -> Quaternion {
  return {
      w() * other.w() - x() * other.x() - y() * other.y() - z() * other.z(),
      w() * other.x() + x() * other.w() + y() * other.z() - z() * other.y(),
      w() * other.y() - x() * other.z() + y() * other.w() + z() * other.x(),
      w() * other.z() + x() * other.y() - y() * other.x() + z() * other.w(),
  };
}

template <typename T>
auto Quaternion<T>::operator*(const Vec3t &vec) const -> Vec3t {
  Vec3t u{x(), y(), z()};
  return 2 * u.dot(vec) * u + (w() * w() - u.dot(u)) * vec + 2 * w() * u.cross(vec);
}

template <typename T>
auto Quaternion<T>::slerp(const Quaternion &other, T t) const -> Quaternion {
  Vec4t q1 = coeffs();
  Vec4t q2 = other.coeffs();

  T dot = q1.dot(q2);
  if (dot < 0) {
    q2  = -q2;
    dot = -dot;
  }

  dot = clamp(dot, static_cast<T>(-1), static_cast<T>(1));

  // If quaternions are very close, use normalized lerp to avoid numerical issues.
  constexpr T kDotThreshold = static_cast<T>(0.9995);
  if (dot > kDotThreshold) {
    Vec4t blended = (static_cast<T>(1) - t) * q1 + t * q2;
    return Quaternion{blended[0], blended[1], blended[2], blended[3]};
  }

  T theta     = std::acos(dot);
  T sin_theta = std::sin(theta);

  // Degenerate case safeguard.
  if (std::abs(sin_theta) < static_cast<T>(1e-8)) {
    Vec4t blended = (static_cast<T>(1) - t) * q1 + t * q2;
    return Quaternion{blended[0], blended[1], blended[2], blended[3]};
  }

  T w1      = std::sin((static_cast<T>(1) - t) * theta) / sin_theta;
  T w2      = std::sin(t * theta) / sin_theta;
  Vec4t out = w1 * q1 + w2 * q2;

  return Quaternion{out[0], out[1], out[2], out[3]};
}

template <typename T>
bool Quaternion<T>::isApprox(const Quaternion &other, T tol) const {
  Vec4t coeffs1 = coeffs();
  Vec4t coeffs2 = other.coeffs();
  return coeffs1.isApprox(coeffs2, tol) or coeffs1.isApprox(-coeffs2, tol);
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const Quaternion<T> &q) {
  return os << "Quaternion(w=" << q.w() << ", x=" << q.x() << ", y=" << q.y() << ", z=" << q.z() << ")";
}

inline auto rpy2rot(float roll, float pitch, float yaw) {
  return Eigen::AngleAxisf(yaw, Vec3f::UnitZ()) * Eigen::AngleAxisf(pitch, Vec3f::UnitY()) *
         Eigen::AngleAxisf(roll, Vec3f::UnitX());
}

inline auto rpy2rot(double roll, double pitch, double yaw) {
  return Eigen::AngleAxisd(yaw, Vec3d::UnitZ()) * Eigen::AngleAxisd(pitch, Vec3d::UnitY()) *
         Eigen::AngleAxisd(roll, Vec3d::UnitX());
}

inline auto rpy2rot(const Vec3f &rpy) { return rpy2rot(rpy.x(), rpy.y(), rpy.z()); }

inline auto rpy2rot(const Vec3d &rpy) { return rpy2rot(rpy.x(), rpy.y(), rpy.z()); }

inline Vec3f mat2rpy(const Mat3f &mat) {
  return {std::atan2(mat(2, 1), mat(2, 2)), std::asin(-mat(2, 0)), std::atan2(mat(1, 0), mat(0, 0))};
}

inline Vec3d mat2rpy(const Mat3d &mat) {
  return {std::atan2(mat(2, 1), mat(2, 2)), std::asin(-mat(2, 0)), std::atan2(mat(1, 0), mat(0, 0))};
}

template <typename T>
Eigen::Matrix<T, 7, 1> interpolateSE3(const Eigen::Matrix<T, 7, 1> &pose0, const Eigen::Matrix<T, 7, 1> &pose1,
                                      T blend) {
  Eigen::Matrix<T, 3, 1> pos0 = pose0.template head<3>();
  Eigen::Matrix<T, 3, 1> pos1 = pose1.template head<3>();
  Quaternion<T> rot0(pose0.template tail<4>());
  Quaternion<T> rot1(pose1.template tail<4>());
  Eigen::Matrix<T, 7, 1> blended_pose;
  blended_pose.template head<3>() = (static_cast<T>(1) - blend) * pos0 + blend * pos1;
  blended_pose.template tail<4>() = rot0.slerp(rot1, static_cast<T>(blend)).coeffs();
  return blended_pose;
}
}  // namespace llu

#endif  // LLU_GEOMETRY_H_
