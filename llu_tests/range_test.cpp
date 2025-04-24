#include <gtest/gtest.h>

#include <llu/range.h>

TEST(LLU_RANGE_TEST, LLU_RANGE_TEST) {
  llu::range_t<double> range{0., 2.};
  ASSERT_EQ(range.lower(), 0.) << "Lower bound should be 0";
  ASSERT_EQ(range.upper(), 2.) << "Upper bound should be 2";
  ASSERT_NEAR(range.mid(), 1., llu::kEPS) << "Mid point should be 1";

  ASSERT_NEAR(llu::clamp(-1., range), 0., llu::kEPS) << "Clamp(-1, {0, 2}) should be 0";
  ASSERT_NEAR(llu::clamp( 0., range), 0., llu::kEPS) << "Clamp( 0, {0, 2}) should be 0";
  ASSERT_NEAR(llu::clamp( 1., range), 1., llu::kEPS) << "Clamp( 1, {0, 2}) should be 1";
  ASSERT_NEAR(llu::clamp( 2., range), 2., llu::kEPS) << "Clamp( 2, {0, 2}) should be 2";
  ASSERT_NEAR(llu::clamp( 3., range), 2., llu::kEPS) << "Clamp( 3, {0, 2}) should be 2";

  ASSERT_NEAR(llu::lerp(range, 0.0), 0.0, llu::kEPS) << "Lerp({0, 2}, 0.0) should be 0.0";
  ASSERT_NEAR(llu::lerp(range, 0.5), 1.0, llu::kEPS) << "Lerp({0, 2}, 0.5) should be 1.0";
  ASSERT_NEAR(llu::lerp(range, 1.0), 2.0, llu::kEPS) << "Lerp({0, 2}, 1.0) should be 2.0";
}
