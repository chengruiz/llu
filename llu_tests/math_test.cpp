#include <gtest/gtest.h>

#include <llu/eigen.h>
#include <llu/math.h>

TEST(LLU_MATH_TEST, LLU_MATH_TEST) {
  ASSERT_NEAR(M_PIf, M_PI, 1e-7) << "M_PIf should be equal to M_PI";

  ASSERT_EQ(llu::square(0), 0) << "Square(0) should be 0";
  ASSERT_EQ(llu::square(2), 4) << "Square(2) should be 4";
  ASSERT_EQ(llu::square(-3), 9) << "Square(-3) should be 9";
  ASSERT_NEAR(llu::square(2.5), 6.25, llu::kEPS) << "Square of 2.5 should be 6.25";

  ASSERT_EQ(llu::median(2, 4, 0), 2) << "Median(2, 4, 0) should be 2";
  ASSERT_EQ(llu::median(2, 4, 6), 4) << "Median(2, 4, 6) should be 4";
  ASSERT_EQ(llu::median(2, 4, 3), 3) << "Median(2, 4, 3) should be 3";
  ASSERT_NEAR(llu::median(-1., 3., -5.), -1., llu::kEPS) << "Median of -1, 3, -5 should be -1";

  ASSERT_NEAR(llu::deg2rad(30.), M_PI / 6., llu::kEPS) << "30 degrees should be PI/6 radians";
  ASSERT_NEAR(llu::deg2rad(60.), M_PI / 3., llu::kEPS) << "60 degrees should be PI/3 radians";
  ASSERT_NEAR(llu::deg2rad(90.), M_PI / 2., llu::kEPS) << "90 degrees should be PI/2 radians";
  ASSERT_NEAR(llu::deg2rad(-30.), -M_PI / 6., llu::kEPS) << "-30 degrees should be -PI/6 radians";
  ASSERT_NEAR(llu::deg2rad(-60.), -M_PI / 3., llu::kEPS) << "-60 degrees should be -PI/3 radians";
  ASSERT_NEAR(llu::deg2rad(-90.), -M_PI / 2., llu::kEPS) << "-90 degrees should be -PI/2 radians";

  ASSERT_NEAR(llu::rad2deg(M_PI / 6.), 30., llu::kEPS) << "PI/6 radians should be 30 degrees";
  ASSERT_NEAR(llu::rad2deg(M_PI / 3.), 60., llu::kEPS) << "PI/3 radians should be 60 degrees";
  ASSERT_NEAR(llu::rad2deg(M_PI / 2.), 90., llu::kEPS) << "PI/2 radians should be 90 degrees";
  ASSERT_NEAR(llu::rad2deg(-M_PI / 6.), -30., llu::kEPS) << "-PI/6 radians should be -30 degrees";
  ASSERT_NEAR(llu::rad2deg(-M_PI / 3.), -60., llu::kEPS) << "-PI/3 radians should be -60 degrees";
  ASSERT_NEAR(llu::rad2deg(-M_PI / 2.), -90., llu::kEPS) << "-PI/2 radians should be -90 degrees";

  ASSERT_NEAR(llu::unit(2.0), 1., llu::kEPS) << "Unit(2.0) should be 1.0";
  ASSERT_NEAR(llu::unit(-2.0), -1., llu::kEPS) << "Unit(-2.0) should be -1.0";
  ASSERT_NEAR(llu::unit(0.0), 0., llu::kEPS) << "Unit(0.0) should be 0.0";

  llu::Vec3d v1(0.1, 0, 0);
  llu::Vec3d v2(0, 0.1, 0);
  llu::Vec3d v3(0, 0., 0.1);
  ASSERT_TRUE(llu::unit(v1).isApprox(llu::Vec3d::UnitX(), 1e-6)) << "Unit({0.1, 0, 0}) should be {1, 0, 0}";
  ASSERT_TRUE(llu::unit(v2).isApprox(llu::Vec3d::UnitY(), 1e-6)) << "Unit({0, 0.1, 0}) should be {0, 1, 0}";
  ASSERT_TRUE(llu::unit(v3).isApprox(llu::Vec3d::UnitZ(), 1e-6)) << "Unit({0, 0, 0.1}) should be {0, 0, 1}";
  ASSERT_TRUE(llu::unit(v1 + v2).isApprox(llu::Vec3d{std::sqrt(0.5), std::sqrt(0.5), 0.}, 1e-6));
  ASSERT_TRUE(llu::unit(v1 + v2 + v3).isApprox(
    llu::Vec3d{std::sqrt(1. / 3.), std::sqrt(1. / 3.), std::sqrt(1. / 3.)}, 1e-6));

  ASSERT_NEAR(llu::interpolate(-1., 1., 0.), -1., llu::kEPS) << "Interpolate(-1, 1, 0) should be -1";
  ASSERT_NEAR(llu::interpolate(-1., 1., 0.25), -0.5, llu::kEPS) << "Interpolate(-1, 1, 0.25) should be -0.5";
  ASSERT_NEAR(llu::interpolate(-1., 1., 0.5), 0., llu::kEPS) << "Interpolate(-1, 1, 0.5) should be 0";
  ASSERT_NEAR(llu::interpolate(-1., 1., 0.75), 0.5, llu::kEPS) << "Interpolate(-1, 1, 0.75) should be 0.5";
  ASSERT_NEAR(llu::interpolate(-1., 1., 1.), 1., llu::kEPS) << "Interpolate(-1, 1, 1) should be 1";

  ASSERT_EQ(llu::ceilDiv(0, 3), 0) << "CeilDiv(0, 3) should be 0";
  ASSERT_EQ(llu::ceilDiv(1, 3), 1) << "CeilDiv(1, 3) should be 1";
  ASSERT_EQ(llu::ceilDiv(2, 3), 1) << "CeilDiv(2, 3) should be 1";
  ASSERT_EQ(llu::ceilDiv(3, 3), 1) << "CeilDiv(3, 3) should be 1";
  ASSERT_EQ(llu::ceilDiv(4, 3), 2) << "CeilDiv(4, 3) should be 2";
  ASSERT_EQ(llu::ceilDiv(5, 3), 2) << "CeilDiv(5, 3) should be 2";
  ASSERT_EQ(llu::ceilDiv(6, 3), 2) << "CeilDiv(6, 3) should be 2";

  ASSERT_EQ(llu::pMod( 0, 2), 0) << "PMod( 0, 2) should be 0";
  ASSERT_EQ(llu::pMod( 1, 2), 1) << "PMod( 1, 2) should be 1";
  ASSERT_EQ(llu::pMod( 2, 2), 0) << "PMod( 2, 2) should be 0";
  ASSERT_EQ(llu::pMod(-3, 2), 1) << "PMod(-3, 2) should be 1";

  ASSERT_NEAR(llu::angleDiff(0., 0.), 0., llu::kEPS) << "AngleDiff(0, 0) should be 0";
  ASSERT_NEAR(llu::angleDiff(0., M_PI*3/2), M_PI/2., llu::kEPS) << "AngleDiff(0, PI*3/2) should be PI/2";
  ASSERT_NEAR(llu::angleDiff(0., -M_PI*3/2), -M_PI/2., llu::kEPS) << "AngleDiff(0, -PI*3/2) should be -PI/2";
  ASSERT_NEAR(llu::angleDiff(0., -M_PI*2), 0., llu::kEPS) << "AngleDiff(0, -PI*2) should be 0";
  ASSERT_NEAR(llu::angleDiff(0., M_PI*2), 0., llu::kEPS) << "AngleDiff(0, PI*2) should be 0";
}
