#ifndef LLU_EIGEN_H_
#define LLU_EIGEN_H_

#include <Eigen/Core>

#define LLU_EIGEN_ALIAS(alias, ...)   \
  using alias    = __VA_ARGS__;       \
  using r##alias = Eigen::Ref<alias>; \
  using c##alias = const Eigen::Ref<const alias>&

namespace llu {
LLU_EIGEN_ALIAS(Arr1f, Eigen::Array<float, 1, 1>);
LLU_EIGEN_ALIAS(Arr2f, Eigen::Array2f);
LLU_EIGEN_ALIAS(Arr3f, Eigen::Array3f);
LLU_EIGEN_ALIAS(Arr4f, Eigen::Array4f);
LLU_EIGEN_ALIAS(Arr5f, Eigen::Array<float, 5, 1>);
LLU_EIGEN_ALIAS(Arr6f, Eigen::Array<float, 6, 1>);
LLU_EIGEN_ALIAS(ArrXf, Eigen::ArrayXf);

LLU_EIGEN_ALIAS(Arr1d, Eigen::Array<double, 1, 1>);
LLU_EIGEN_ALIAS(Arr2d, Eigen::Array2d);
LLU_EIGEN_ALIAS(Arr3d, Eigen::Array3d);
LLU_EIGEN_ALIAS(Arr4d, Eigen::Array4d);
LLU_EIGEN_ALIAS(Arr5d, Eigen::Array<double, 5, 1>);
LLU_EIGEN_ALIAS(Arr6d, Eigen::Array<double, 6, 1>);
LLU_EIGEN_ALIAS(ArrXd, Eigen::ArrayXd);

LLU_EIGEN_ALIAS(Vec1f, Eigen::Matrix<float, 1, 1>);
LLU_EIGEN_ALIAS(Vec2f, Eigen::Vector2f);
LLU_EIGEN_ALIAS(Vec3f, Eigen::Vector3f);
LLU_EIGEN_ALIAS(Vec4f, Eigen::Vector4f);
LLU_EIGEN_ALIAS(Vec5f, Eigen::Matrix<float, 5, 1>);
LLU_EIGEN_ALIAS(Vec6f, Eigen::Matrix<float, 6, 1>);
LLU_EIGEN_ALIAS(VecXf, Eigen::VectorXf);

LLU_EIGEN_ALIAS(Vec1d, Eigen::Matrix<double, 1, 1>);
LLU_EIGEN_ALIAS(Vec2d, Eigen::Vector2d);
LLU_EIGEN_ALIAS(Vec3d, Eigen::Vector3d);
LLU_EIGEN_ALIAS(Vec4d, Eigen::Vector4d);
LLU_EIGEN_ALIAS(Vec5d, Eigen::Matrix<double, 5, 1>);
LLU_EIGEN_ALIAS(Vec6d, Eigen::Matrix<double, 6, 1>);
LLU_EIGEN_ALIAS(VecXd, Eigen::VectorXd);

LLU_EIGEN_ALIAS(Mat2f, Eigen::Matrix2f);
LLU_EIGEN_ALIAS(Mat3f, Eigen::Matrix3f);
LLU_EIGEN_ALIAS(Mat4f, Eigen::Matrix4f);
LLU_EIGEN_ALIAS(MatXf, Eigen::MatrixXf);

LLU_EIGEN_ALIAS(Mat2d, Eigen::Matrix2d);
LLU_EIGEN_ALIAS(Mat3d, Eigen::Matrix3d);
LLU_EIGEN_ALIAS(Mat4d, Eigen::Matrix4d);
LLU_EIGEN_ALIAS(MatXd, Eigen::MatrixXd);
}  // namespace llu

#endif  // LLU_EIGEN_H_
