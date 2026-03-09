#include <gtest/gtest.h>

#include <cmath>

#include <llu/eigen.h>
#include <llu/math.h>

TEST(LLU_MATH_TEST, PiFloatMatchesTheDoublePrecisionConstant) {
  EXPECT_NEAR(M_PIf, M_PI, 1e-7);
}

TEST(LLU_MATH_TEST, SquareAndMedianHandleIntegralAndFloatingValues) {
  EXPECT_EQ(llu::square(0), 0);
  EXPECT_EQ(llu::square(2), 4);
  EXPECT_EQ(llu::square(-3), 9);
  EXPECT_NEAR(llu::square(2.5), 6.25, llu::kEPS);

  EXPECT_EQ(llu::median(2, 4, 0), 2);
  EXPECT_EQ(llu::median(2, 4, 6), 4);
  EXPECT_EQ(llu::median(2, 4, 3), 3);
  EXPECT_NEAR(llu::median(-1., 3., -5.), -1., llu::kEPS);
}

TEST(LLU_MATH_TEST, DegreeAndRadianConvertersAreConsistent) {
  EXPECT_NEAR(llu::deg2rad(30.), M_PI / 6., llu::kEPS);
  EXPECT_NEAR(llu::deg2rad(60.), M_PI / 3., llu::kEPS);
  EXPECT_NEAR(llu::deg2rad(-90.), -M_PI / 2., llu::kEPS);

  EXPECT_NEAR(llu::rad2deg(M_PI / 6.), 30., llu::kEPS);
  EXPECT_NEAR(llu::rad2deg(M_PI / 3.), 60., llu::kEPS);
  EXPECT_NEAR(llu::rad2deg(-M_PI / 2.), -90., llu::kEPS);
}

TEST(LLU_MATH_TEST, UnitNormalizesScalarsAndVectors) {
  EXPECT_NEAR(llu::unit(2.0), 1., llu::kEPS);
  EXPECT_NEAR(llu::unit(-2.0), -1., llu::kEPS);
  EXPECT_NEAR(llu::unit(0.0), 0., llu::kEPS);

  llu::Vec3d v1(0.1, 0, 0);
  llu::Vec3d v2(0, 0.1, 0);
  llu::Vec3d v3(0, 0., 0.1);
  EXPECT_TRUE(llu::unit(v1).isApprox(llu::Vec3d::UnitX(), 1e-6));
  EXPECT_TRUE(llu::unit(v2).isApprox(llu::Vec3d::UnitY(), 1e-6));
  EXPECT_TRUE(llu::unit(v3).isApprox(llu::Vec3d::UnitZ(), 1e-6));
  EXPECT_TRUE(llu::unit(v1 + v2).isApprox(llu::Vec3d{std::sqrt(0.5), std::sqrt(0.5), 0.}, 1e-6));
  EXPECT_TRUE(
      llu::unit(v1 + v2 + v3).isApprox(llu::Vec3d{std::sqrt(1. / 3.), std::sqrt(1. / 3.), std::sqrt(1. / 3.)}, 1e-6));
}

TEST(LLU_MATH_TEST, LerpInterpolatesAndClampsFactorsToTheUnitInterval) {
  EXPECT_NEAR(llu::lerp(-1., 1., -0.5), -1., llu::kEPS);
  EXPECT_NEAR(llu::lerp(-1., 1., 0.25), -0.5, llu::kEPS);
  EXPECT_NEAR(llu::lerp(-1., 1., 0.5), 0., llu::kEPS);
  EXPECT_NEAR(llu::lerp(-1., 1., 0.75), 0.5, llu::kEPS);
  EXPECT_NEAR(llu::lerp(-1., 1., 1.5), 1., llu::kEPS);
}

TEST(LLU_MATH_TEST, IntegerHelpersHandleCeilDivisionAndPositiveModulo) {
  EXPECT_EQ(llu::ceilDiv(0, 3), 0);
  EXPECT_EQ(llu::ceilDiv(1, 3), 1);
  EXPECT_EQ(llu::ceilDiv(2, 3), 1);
  EXPECT_EQ(llu::ceilDiv(3, 3), 1);
  EXPECT_EQ(llu::ceilDiv(4, 3), 2);
  EXPECT_EQ(llu::ceilDiv(5, 3), 2);
  EXPECT_EQ(llu::ceilDiv(6, 3), 2);

  EXPECT_EQ(llu::pMod(0, 2), 0);
  EXPECT_EQ(llu::pMod(1, 2), 1);
  EXPECT_EQ(llu::pMod(2, 2), 0);
  EXPECT_EQ(llu::pMod(-3, 2), 1);
}

TEST(LLU_MATH_TEST, AngleDiffWrapsAcrossTheBranchCut) {
  EXPECT_NEAR(llu::angleDiff(0., 0.), 0., llu::kEPS);
  EXPECT_NEAR(llu::angleDiff(0., M_PI * 3 / 2), M_PI / 2., llu::kEPS);
  EXPECT_NEAR(llu::angleDiff(0., -M_PI * 3 / 2), -M_PI / 2., llu::kEPS);
  EXPECT_NEAR(llu::angleDiff(0., -M_PI * 2), 0., llu::kEPS);
  EXPECT_NEAR(llu::angleDiff(0., M_PI * 2), 0., llu::kEPS);
}

TEST(LLU_MATH_TEST, SigmoidMatchesExpectedAnalyticalValues) {
  llu::Arr3d values;
  values << -1., 0., 1.;

  llu::Arr3d expected;
  expected << 1. / (1. + std::exp(1.)), 0.5, 1. / (1. + std::exp(-1.));

  EXPECT_TRUE(llu::sigmoid(values).isApprox(expected, 1e-12));
}
