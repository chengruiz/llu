#include <gtest/gtest.h>

#include <llu/range.h>

TEST(LLU_RANGE_TEST, AccessorsExposeBoundsAndMidpoint) {
  const llu::range_t<double> range{0., 2.};

  EXPECT_EQ(range.lower(), 0.);
  EXPECT_EQ(range.upper(), 2.);
  EXPECT_NEAR(range.mid(), 1., llu::kEPS);
}

TEST(LLU_RANGE_TEST, ClampConstrainsValuesToTheGivenRange) {
  const llu::range_t<double> range{0., 2.};

  EXPECT_NEAR(llu::clamp(-1., range), 0., llu::kEPS);
  EXPECT_NEAR(llu::clamp(0., range), 0., llu::kEPS);
  EXPECT_NEAR(llu::clamp(1., range), 1., llu::kEPS);
  EXPECT_NEAR(llu::clamp(2., range), 2., llu::kEPS);
  EXPECT_NEAR(llu::clamp(3., range), 2., llu::kEPS);
}

TEST(LLU_RANGE_TEST, LerpInterpolatesAcrossTheRangeAndClampsFactors) {
  const llu::range_t<double> range{0., 2.};

  EXPECT_NEAR(llu::lerp(range, 0.0), 0.0, llu::kEPS);
  EXPECT_NEAR(llu::lerp(range, 0.5), 1.0, llu::kEPS);
  EXPECT_NEAR(llu::lerp(range, 1.0), 2.0, llu::kEPS);
}
