#include <array>

#include <gtest/gtest.h>
#include <Eigen/Geometry>

#include <llu/const.h>
#include <llu/geometry.h>

namespace {
void expectEquivalentRotation(const Eigen::Quaterniond &expected, const llu::Quatd &actual) {
  const auto normalized_expected = expected.normalized();

  EXPECT_TRUE(actual.isApprox(llu::Quatd{normalized_expected}, 1e-6));
  EXPECT_TRUE(actual.matrix().isApprox(normalized_expected.toRotationMatrix(), 1e-6));
}

void expectSlerpMatchesEigen(const Eigen::Quaterniond &start, const Eigen::Quaterniond &end, double t,
                             double prec = 1e-6) {
  const auto expected = start.slerp(t, end);
  const auto actual   = llu::Quatd{start}.slerp(llu::Quatd{end}, t);

  EXPECT_TRUE(actual.isApprox(llu::Quatd{expected}, prec));
  EXPECT_TRUE(actual.matrix().isApprox(expected.toRotationMatrix(), prec));
}
}  // namespace

TEST(LLU_GEOMETRY_TEST, ConstructorsNormalizeCoefficientsAndPreserveRotation) {
  const auto eigen_q        = Eigen::Quaterniond(0.1, 0.2, 0.3, 0.4);
  const auto llu_q          = llu::Quatd(0.1, 0.2, 0.3, 0.4);
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
  const auto q1     = Eigen::Quaterniond(0.1, 0.2, -0.3, 0.4).normalized();
  const auto q2     = Eigen::Quaterniond(0.7, 0.6, 0.5, 0.4).normalized();
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

TEST(LLU_GEOMETRY_TEST, AxisConstructorsMatchEigenAndSetIdentityResetsRotation) {
  expectEquivalentRotation(Eigen::Quaterniond(Eigen::AngleAxisd(0.3, Eigen::Vector3d::UnitX())),
                           llu::Quatd::fromRoll(0.3));
  expectEquivalentRotation(Eigen::Quaterniond(Eigen::AngleAxisd(-0.4, Eigen::Vector3d::UnitY())),
                           llu::Quatd::fromPitch(-0.4));
  expectEquivalentRotation(Eigen::Quaterniond(Eigen::AngleAxisd(1.1, Eigen::Vector3d::UnitZ())),
                           llu::Quatd::fromYaw(1.1));

  auto q = llu::Quatd::fromEulerAngles(0.2, -0.3, 0.4);
  q.setIdentity();

  EXPECT_TRUE(q.isApprox(llu::Quatd{}, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, QuaternionRotatesVectorsLikeEigen) {
  const auto eigen_q = Eigen::Quaterniond(-0.9, 0.3, 0.0, 0.1).normalized();
  const auto llu_q   = llu::Quatd{eigen_q};
  const llu::Vec3d vector{1., 2., 3.};

  EXPECT_TRUE((llu_q * vector).isApprox(eigen_q * vector, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, RotationVectorMatchesAngleAxisForEquivalentQuaternions) {
  const auto axis          = Eigen::Vector3d(1.0, -2.0, 3.0).normalized();
  const auto angle         = 1.2;
  const auto eigen_q       = Eigen::Quaterniond(Eigen::AngleAxisd(angle, axis));
  const auto expected      = axis * angle;
  const auto negated_llu_q = llu::Quatd{-eigen_q.w(), -eigen_q.x(), -eigen_q.y(), -eigen_q.z()};

  EXPECT_TRUE(llu::Quatd{eigen_q}.rotationVector().isApprox(expected, 1e-6));
  EXPECT_TRUE(negated_llu_q.rotationVector().isApprox(expected, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, Rotation6dDefaultsToColumnMajorEncoding) {
  const auto identity = llu::Quatd{};
  llu::Vec6d expected_identity;
  expected_identity << 1., 0., 0., 0., 1., 0.;

  EXPECT_TRUE(identity.rotation6d().isApprox(expected_identity, 1e-6));
  EXPECT_TRUE(identity.rotation6d(llu::Rotation6dOrder::kColumnMajor).isApprox(expected_identity, 1e-6));

  const auto q = llu::Quatd::fromEulerAngles(0.3, -0.4, 1.1);
  const auto m = q.matrix();
  llu::Vec6d expected;
  expected << m(0, 0), m(1, 0), m(2, 0), m(0, 1), m(1, 1), m(2, 1);

  EXPECT_TRUE(q.rotation6d().isApprox(expected, 1e-6));
  EXPECT_TRUE(q.rotation6d(llu::Rotation6dOrder::kColumnMajor).isApprox(expected, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, Rotation6dSupportsRowMajorEncoding) {
  const auto q = llu::Quatd::fromEulerAngles(0.3, -0.4, 1.1);
  const auto m = q.matrix();
  llu::Vec6d expected;
  expected << m(0, 0), m(0, 1), m(1, 0), m(1, 1), m(2, 0), m(2, 1);

  EXPECT_TRUE(q.rotation6d(llu::Rotation6dOrder::kRowMajor).isApprox(expected, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, SlerpMatchesEigenAcrossInterpolationFactors) {
  const auto start = Eigen::Quaterniond(0.8, -0.1, 0.4, 0.4).normalized();
  const auto end   = Eigen::Quaterniond(-0.2, 0.9, -0.1, 0.3).normalized();

  expectSlerpMatchesEigen(start, end, 0.0);
  expectSlerpMatchesEigen(start, end, 0.25);
  expectSlerpMatchesEigen(start, end, 0.5);
  expectSlerpMatchesEigen(start, end, 1.0);
}

TEST(LLU_GEOMETRY_TEST, SlerpTreatsNegatedQuaternionAsSameRotation) {
  const auto start = Eigen::Quaterniond(0.1, -0.4, 0.2, 0.8).normalized();
  const Eigen::Quaterniond same_rotation{-start.w(), -start.x(), -start.y(), -start.z()};
  const auto llu_start         = llu::Quatd{start};
  const auto llu_same_rotation = llu::Quatd{same_rotation};

  EXPECT_TRUE(llu_start.slerp(llu_same_rotation, 0.0).isApprox(llu_start, 1e-6));
  EXPECT_TRUE(llu_start.slerp(llu_same_rotation, 0.5).isApprox(llu_start, 1e-6));
  EXPECT_TRUE(llu_start.slerp(llu_same_rotation, 1.0).isApprox(llu_start, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, SlerpMatchesEigenForNearlyIdenticalQuaternions) {
  const auto start = Eigen::Quaterniond::Identity();
  const auto end   = Eigen::Quaterniond(Eigen::AngleAxisd(1e-6, Eigen::Vector3d::UnitZ()));

  expectSlerpMatchesEigen(start, end, 0.5, 1e-9);
}

TEST(LLU_GEOMETRY_TEST, RpyHelpersConvertBetweenAnglesAndMatrices) {
  const llu::Vec3d rpy{0.2, -0.1, 0.7};
  const auto rotation = llu::rpy2rot(rpy);

  EXPECT_TRUE(rotation.matrix().isApprox(llu::Quatd::fromEulerAngles(rpy).matrix(), 1e-6));
  EXPECT_TRUE(llu::mat2rpy(rotation.matrix()).isApprox(rpy, 1e-6));
}

TEST(LLU_GEOMETRY_TEST, InterpolateSE3BlendsTranslationAndRotation) {
  Eigen::Matrix<double, 7, 1> pose0;
  pose0 << 0., 1., -2., 1., 0., 0., 0.;

  Eigen::Matrix<double, 7, 1> pose1;
  pose1.head<3>() = llu::Vec3d{4., -3., 2.};
  const auto end_rotation = Eigen::Quaterniond(Eigen::AngleAxisd(M_PI / 2, Eigen::Vector3d::UnitZ()));
  pose1.tail<4>() << end_rotation.w(), end_rotation.x(), end_rotation.y(), end_rotation.z();

  const auto blended           = llu::interpolateSE3(pose0, pose1, 0.25);
  const auto expected_rotation = Eigen::Quaterniond::Identity().slerp(0.25, end_rotation);

  EXPECT_TRUE(blended.head<3>().isApprox(llu::Vec3d{1., 0., -1.}, 1e-6));
  EXPECT_TRUE(llu::Quatd{blended.tail<4>()}.isApprox(llu::Quatd{expected_rotation}, 1e-6));
}
