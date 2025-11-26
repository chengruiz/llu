#include <gtest/gtest.h>

#include <llu/eigen.h>

TEST(LLU_EIGEN_TEST, GetNonFiniteIndices) {
  // Test with all finite values
  llu::ArrXf arr_finite(5);
  arr_finite << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f;
  auto indices_finite = llu::getNonFiniteIndices<float>(arr_finite);
  EXPECT_TRUE(indices_finite.empty());

  // Test with NaN
  llu::ArrXf arr_nan(3);
  arr_nan << 1.0f, std::numeric_limits<float>::quiet_NaN(), 3.0f;
  auto indices_nan = llu::getNonFiniteIndices<float>(arr_nan);
  ASSERT_EQ(indices_nan.size(), 1);
  EXPECT_EQ(indices_nan[0], 1);

  // Test with Infinity
  llu::ArrXf arr_inf(3);
  arr_inf << std::numeric_limits<float>::infinity(), 2.0f, 3.0f;
  auto indices_inf = llu::getNonFiniteIndices<float>(arr_inf);
  ASSERT_EQ(indices_inf.size(), 1);
  EXPECT_EQ(indices_inf[0], 0);

  // Test with -Infinity
  llu::ArrXf arr_neg_inf(3);
  arr_neg_inf << 1.0f, 2.0f, -std::numeric_limits<float>::infinity();
  auto indices_neg_inf = llu::getNonFiniteIndices<float>(arr_neg_inf);
  ASSERT_EQ(indices_neg_inf.size(), 1);
  EXPECT_EQ(indices_neg_inf[0], 2);

  // Test with mixed non-finite values (double)
  llu::ArrXd arr_mixed(5);
  arr_mixed << 1.0, std::numeric_limits<double>::quiet_NaN(), 
               std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity(), 5.0;
  auto indices_mixed = llu::getNonFiniteIndices<double>(arr_mixed);
  ASSERT_EQ(indices_mixed.size(), 3);
  EXPECT_EQ(indices_mixed[0], 1);
  EXPECT_EQ(indices_mixed[1], 2);
  EXPECT_EQ(indices_mixed[2], 3);

  // Test with empty array
  llu::ArrXf arr_empty(0);
  auto indices_empty = llu::getNonFiniteIndices<float>(arr_empty);
  EXPECT_TRUE(indices_empty.empty());
}
