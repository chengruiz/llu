#include <gtest/gtest.h>

#include <string>

#include <llu/macro.h>

namespace {
template <typename T>
constexpr bool floatingPointAssertionCompiles() {
  LLU_ASSERT_FLOATING_POINT(T);
  return true;
}

template <typename T>
constexpr bool intAssertionCompiles() {
  LLU_ASSERT_INT(T);
  return true;
}
}  // namespace

TEST(LLU_MACRO_TEST, ToStrStringifiesTokenSequences) {
  EXPECT_EQ(std::string{LLU_TO_STR(alpha)}, "alpha");
  EXPECT_EQ(std::string{LLU_TO_STR(1 + 2)}, "1 + 2");
}

TEST(LLU_MACRO_TEST, FileLineIncludesTheCurrentFileNameAndLineSeparator) {
  const std::string fileline = LLU_FILELINE();

  ASSERT_FALSE(fileline.empty());
  EXPECT_EQ(fileline.front(), '[');
  EXPECT_EQ(fileline.back(), ']');
  EXPECT_NE(fileline.find(__FILE_NAME__), std::string::npos);
  EXPECT_NE(fileline.find(':'), std::string::npos);
}

TEST(LLU_MACRO_TEST, TypeAssertionMacrosCompileForSupportedTypes) {
  EXPECT_TRUE((floatingPointAssertionCompiles<float>()));
  EXPECT_TRUE((floatingPointAssertionCompiles<double>()));
  EXPECT_TRUE((intAssertionCompiles<int>()));
  EXPECT_TRUE((intAssertionCompiles<long>()));
}
