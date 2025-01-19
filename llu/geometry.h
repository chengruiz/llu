#ifndef LLU_GEOMETRY_H_
#define LLU_GEOMETRY_H_

#include <Eigen/Dense>

#include <llu/eigen.h>
#include <llu/macro.h>

namespace llu {
template<typename T>
class Quaternion {
  LLU_EIGEN_ALIAS(Vec3T, Eigen::Matrix<T, 3, 1>);
  LLU_EIGEN_ALIAS(Vec4T, Eigen::Matrix<T, 4, 1>);
  LLU_EIGEN_ALIAS(Mat3T, Eigen::Matrix<T, 3, 3>);
  static constexpr T PI = static_cast<T>(M_PI);

public:
  LLU_ASSERT_FP(T);
  Quaternion(): data_{1., 0., 0., 0.} {}
  Quaternion(const Quaternion &q) : data_(q.data_) {}
  Quaternion(T w, T x, T y, T z) : data_{w, x, y, z} { normalize(); }
  explicit Quaternion(const std::array<T, 4> &data) : data_(data) { normalize(); }
  explicit Quaternion(cVec4T q) : data_{q[0], q[1], q[2], q[3]} {}
  explicit Quaternion(const Eigen::Quaternion<T> &q) : data_{q.w(), q.x(), q.y(), q.z()} {}
  static Quaternion fromMatrix(cMat3T mat);
  static Quaternion fromRoll(T roll) { return {std::cos(roll / 2), std::sin(roll / 2), 0., 0.}; }
  static Quaternion fromPitch(T pitch) { return {std::cos(pitch / 2), 0., std::sin(pitch / 2), 0.}; }
  static Quaternion fromYaw(T yaw) { return {std::cos(yaw / 2), 0., 0., std::sin(yaw / 2)}; }
  static Quaternion fromEulerAngles(const Vec3T &rpy) { return fromEulerAngles(rpy.x(), rpy.y(), rpy.z()); }
  static Quaternion fromEulerAngles(const std::array<T, 3> &rpy) { return fromEulerAngles(rpy[0], rpy[1], rpy[2]); }
  static Quaternion fromEulerAngles(T roll, T pitch, T yaw) { return fromYaw(yaw) * fromPitch(pitch) * fromRoll(roll); }

  void setIdentity() { data_ = {1., 0., 0., 0.}; }
  [[nodiscard]] T w() const { return data_[0]; }
  [[nodiscard]] T x() const { return data_[1]; }
  [[nodiscard]] T y() const { return data_[2]; }
  [[nodiscard]] T z() const { return data_[3]; }
  [[nodiscard]] Vec4T coeffs() const { return {w(), x(), y(), z()}; }
  [[nodiscard]] Quaternion inverse() const { return {w(), -x(), -y(), -z()}; }
  [[nodiscard]] Mat3T matrix() const;
  [[nodiscard]] Vec3T eulerAngles() const;

  [[nodiscard]] Quaternion operator*(const Quaternion &other) const;
  [[nodiscard]] Vec3T operator*(const Vec3T &vec) const;
  [[nodiscard]] bool isApprox(const Quaternion &other, T prec) const;

private:
  std::array<T, 4> data_;

  [[nodiscard]] T norm() const { return std::sqrt(squaredNorm()); }
  [[nodiscard]] T squaredNorm() const { return w() * w() + x() * x() + y() * y() + z() * z(); }
  Quaternion &normalize() { return operator/=(norm()); }

  Quaternion operator*(T coef) const { return {w() * coef, x() * coef, y() * coef, z() * coef}; }
  Quaternion &operator*=(T coef);
  Quaternion operator/(double coef) const { return operator*(1 / coef); }
  Quaternion &operator/=(double coef) { return operator*=(1 / coef); }
};

using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;

template<typename T>
auto Quaternion<T>::fromMatrix(cMat3T mat) -> Quaternion {
  // https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion

  T t = mat.trace();
  T r = std::sqrt(1 + t);
  T s = 1 / (2 * r);

  return {
    r / 2,
    (mat(2, 1) - mat(1, 2)) * s,
    (mat(0, 2) - mat(2, 0)) * s,
    (mat(1, 0) - mat(0, 1)) * s,
  };
}

template<typename T>
auto Quaternion<T>::operator*=(T coef) -> Quaternion & {
  data_[0] *= coef;
  data_[1] *= coef;
  data_[2] *= coef;
  data_[3] *= coef;
  return *this;
}

template<typename T>
auto Quaternion<T>::operator*(const Quaternion &other) const -> Quaternion {
  return {
    w() * other.w() - x() * other.x() - y() * other.y() - z() * other.z(),
    w() * other.x() + x() * other.w() + y() * other.z() - z() * other.y(),
    w() * other.y() - x() * other.z() + y() * other.w() + z() * other.x(),
    w() * other.z() + x() * other.y() - y() * other.x() + z() * other.w(),
  };
}

template<typename T> bool Quaternion<T>::isApprox(const Quaternion &other, T prec) const {
  Vec4T coeffs1 = coeffs();
  Vec4T coeffs2 = other.coeffs();
  return coeffs1.isApprox(coeffs2, prec) or coeffs1.isApprox(-coeffs2, prec);
}

template<typename T>
auto Quaternion<T>::eulerAngles() const -> Vec3T {
  return {
    std::atan2(
      2 * (w() * x() + y() * z()),
      1 - 2 * (x() * x() + y() * y())
    ),
    -PI / 2 + 2 * std::atan2(
      std::sqrt(1 + 2 * (w() * y() - x() * z())),
      std::sqrt(1 - 2 * (w() * y() - x() * z()))
    ),
    std::atan2(
      2 * (w() * z() + x() * y()),
      1 - 2 * (y() * y() + z() * z())
    )
  };
}

template<typename T>
auto Quaternion<T>::matrix() const -> Mat3T {
  Mat3T R;
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

template<typename T>
auto Quaternion<T>::operator*(const Vec3T &vec) const -> Vec3T {
  Vec3T u{x(), y(), z()};
  return 2 * u.dot(vec) * u + (w() * w() - u.dot(u)) * vec + 2 * w() * u.cross(vec);
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const Quaternion<T> &q) {
  return os << "Quaternion(w=" << q.w() << ", x=" << q.x() << ", y=" << q.y() << ", z=" << q.z() << ")";
}

inline auto rpy2rot(float roll, float pitch, float yaw) {
  return Eigen::AngleAxisf(yaw, Vec3f::UnitZ()) *
      Eigen::AngleAxisf(pitch, Vec3f::UnitY()) *
      Eigen::AngleAxisf(roll, Vec3f::UnitX());
}

inline auto rpy2rot(double roll, double pitch, double yaw) {
  return Eigen::AngleAxisd(yaw, Vec3d::UnitZ()) *
      Eigen::AngleAxisd(pitch, Vec3d::UnitY()) *
      Eigen::AngleAxisd(roll, Vec3d::UnitX());
}

inline auto rpy2rot(const Vec3f &rpy) {
  return rpy2rot(rpy.x(), rpy.y(), rpy.z());
}

inline auto rpy2rot(const Vec3d &rpy) {
  return rpy2rot(rpy.x(), rpy.y(), rpy.z());
}

inline Vec3f mat2rpy(const Mat3f &mat) {
  return {
    std::atan2(mat(2, 1), mat(2, 2)),
    std::asin(-mat(2, 0)),
    std::atan2(mat(1, 0), mat(0, 0))
  };
}

inline Vec3d mat2rpy(const Mat3d &mat) {
  return {
    std::atan2(mat(2, 1), mat(2, 2)),
    std::asin(-mat(2, 0)),
    std::atan2(mat(1, 0), mat(0, 0))
  };
}
} // namespace llu


#endif  // LLU_GEOMETRY_H_
