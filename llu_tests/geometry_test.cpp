#include <gtest/gtest.h>

#include <array>

#include <Eigen/Geometry>

#include <llu/const.h>
#include <llu/geometry.h>

namespace {
void expectEquivalentRotation(const Eigen::Quaterniond &expected, const llu::Quatd &actual) {
  const auto normalized_expected = expected.normalized();

  EXPECT_TRUE(actual.isApprox(llu::Quatd{normalized_expected}, 1e-6));
  EXPECT_TRUE(actual.matrix().isApprox(normalized_expected.toRotationMatrix(), 1e-6));
}
}  // namespace

TEST(LLU_GEOMETRY_TEST, ConstructorsNormalizeCoefficientsAndPreserveRotation) {
  const auto eigen_q = Eigen::Quaterniond(0.1, 0.2, 0.3, 0.4);
  const auto llu_q = llu::Quatd(0.1, 0.2, 0.3, 0.4);
  const auto llu_from_array = llu::Quatd(std::array<double, 4>{0.1, 0.2, 0.3, 0.4});
  const auto llu_from_eigen = llu::Quatd(eigen_q);

  expectEquivalentRotation(eigen_q, llu_q);
  EXPECT_TRUE(llu_from_array.isApprox(llu_q, llu::kEPS));
  EXPECT_TRUE(llu_from_eigen.isApprox(llu_q, llu::kEPS));

  const auto coeffs = llu_q.coeffs();
  EXPECT_NEAR(coeffs[0], llu_q.w(), llu::kEPS);
  EXPECT_NEAR(coeffs[1], llu_q.x(), llu::kEPS);
  EXPECT_NEAR(coeffs[2], llu_q.y(), llu::kEPS);
  EXPECT_NEAR(coeffs[3], llu_q.z(), llu::kEPS);
}

TEST(LLU_GEOMETRY_TEST, InverseAndMultiplicationMatchEigenQuaternionSemantics) {
  const auto q1 = Eigen::Quaterniond(0.1, 0.2, -0.3, 0.4).normalized();
  const auto q2 = Eigen::Quaterniond(0.7, 0.6, 0.5, 0.4).normalized();
  const auto llu_q1 = llu::Quatd{q1};
  const auto llu_q2 = llu::Quatd{q2};

  EXPECT_TRUE(llu_q1.inverse().isApprox(llu::Quatd{q1.inverse()}, 1e-6));
  EXPECT_TRUE((llu_q1 * llu_q2).isApprox(llu::Quatd{q1 * q2}, 1e-6));
  EXPECT_TRUE((llu_q1 * llu_q1.inverse()).isApprox(llu::Quatd{}, 1e-6));
  EXPECT_TRUE((llu_q1.inverse() * llu_q1).isApprox(llu::Quatd{}, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, EulerAndMatrixConversionsRoundTrip) {
  const llu::Vec3d rpy{0.3, -0.4, 1.1};
  const auto q = llu::Quatd::fromEulerAngles(rpy);

  EXPECT_TRUE(llu::Quatd::fromEulerAngles(std::array<double, 3>{0.3, -0.4, 1.1}).isApprox(q, 1e-6));
  EXPECT_TRUE(llu::Quatd::fromMatrix(q.matrix()).isApprox(q, 1e-6));
  EXPECT_TRUE(llu::Quatd::fromEulerAngles(q.eulerAngles()).isApprox(q, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, QuaternionRotatesVectorsLikeEigen) {
  const auto eigen_q = Eigen::Quaterniond(-0.9, 0.3, 0.0, 0.1).normalized();
  const auto llu_q = llu::Quatd{eigen_q};
  const llu::Vec3d vector{1., 2., 3.};

  EXPECT_TRUE((llu_q * vector).isApprox(eigen_q * vector, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, RpyHelpersConvertBetweenAnglesAndMatrices) {
  const llu::Vec3d rpy{0.2, -0.1, 0.7};
  const auto rotation = llu::rpy2rot(rpy);

  EXPECT_TRUE(rotation.matrix().isApprox(llu::Quatd::fromEulerAngles(rpy).matrix(), 1e-6));
  EXPECT_TRUE(llu::mat2rpy(rotation.matrix()).isApprox(rpy, 1e-6));
}
