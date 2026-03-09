#include <gtest/gtest.h>

#include <limits>
#include <vector>

#include <llu/eigen.h>

TEST(LLU_EIGEN_TEST, FiniteArraysReturnNoIndices) {
  llu::ArrXf arr_finite(5);
  arr_finite << 1.0f, 2.0f, 3.0f, 4.0f, 5.0f;

  EXPECT_TRUE(llu::getNonFiniteIndices(arr_finite).empty());

  llu::ArrXf arr_empty(0);
  EXPECT_TRUE(llu::getNonFiniteIndices(arr_empty).empty());
}

TEST(LLU_EIGEN_TEST, ArraysReportNanAndInfinityIndices) {
  llu::ArrXf arr_nan(3);
  arr_nan << 1.0f, std::numeric_limits<float>::quiet_NaN(), 3.0f;
  EXPECT_EQ(llu::getNonFiniteIndices(arr_nan), (std::vector<Eigen::Index>{1}));

  llu::ArrXf arr_inf(3);
  arr_inf << std::numeric_limits<float>::infinity(), 2.0f, 3.0f;
  EXPECT_EQ(llu::getNonFiniteIndices(arr_inf), (std::vector<Eigen::Index>{0}));

  llu::ArrXf arr_neg_inf(3);
  arr_neg_inf << 1.0f, 2.0f, -std::numeric_limits<float>::infinity();
  EXPECT_EQ(llu::getNonFiniteIndices(arr_neg_inf), (std::vector<Eigen::Index>{2}));
}

TEST(LLU_EIGEN_TEST, MixedDoubleArraysPreserveIndexOrder) {
  llu::ArrXd arr_mixed(5);
  arr_mixed << 1.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity(),
      -std::numeric_limits<double>::infinity(),
      5.0;

  EXPECT_EQ(llu::getNonFiniteIndices(arr_mixed), (std::vector<Eigen::Index>{1, 2, 3}));
}

TEST(LLU_EIGEN_TEST, MatricesUseEigenStorageOrderForIndices) {
  llu::Mat3d mat_mixed;
  mat_mixed << 1.0,
      2.0,
      std::numeric_limits<double>::infinity(),
      4.0,
      std::numeric_limits<double>::quiet_NaN(),
      6.0,
      7.0,
      8.0,
      9.0;

  EXPECT_EQ(llu::getNonFiniteIndices(mat_mixed), (std::vector<Eigen::Index>{4, 6}));
}
