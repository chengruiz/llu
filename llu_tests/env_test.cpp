#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <llu/env.h>

namespace {
template <typename Callback>
std::string captureWhat(Callback &&callback) {
  try {
    callback();
  } catch (const std::exception &error) {
    return error.what();
  }
  return {};
}
}  // namespace

TEST(LLU_ENV_TEST, MissingVariablesReturnFalseForSupportedTypes) {
  std::string str_val;
  long int_val = 0;
  bool bool_val = false;
  std::vector<std::string> paths;

  EXPECT_FALSE(llu::getenv("LLU_ENV_TEST_MISSING_STRING", str_val));
  EXPECT_FALSE(llu::getenv("LLU_ENV_TEST_MISSING_INTEGER", int_val));
  EXPECT_FALSE(llu::getenv("LLU_ENV_TEST_MISSING_BOOL", bool_val));
  EXPECT_FALSE(llu::getenv("LLU_ENV_TEST_MISSING_PATHS", paths));
}

TEST(LLU_ENV_TEST, StringAndIntegerDecodersReadPresentVariables) {
  std::string str_val;
  long int_val = 0;

  ASSERT_EQ(::setenv("LLU_ENV_TEST_STRING", "value", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_INTEGER", "123", 1), 0);

  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_STRING", str_val));
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_INTEGER", int_val));

  EXPECT_EQ(str_val, "value");
  EXPECT_EQ(int_val, 123);
}

TEST(LLU_ENV_TEST, BoolDecoderAcceptsCommonTruthyAndFalseyValues) {
  bool bool_val = false;

  ASSERT_EQ(::setenv("LLU_ENV_TEST_TRUE_1", "1", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_TRUE_TRUE", "true", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_TRUE_YES", "yes", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_TRUE_ON", "on", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_FALSE_0", "0", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_FALSE_FALSE", "false", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_FALSE_NO", "no", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_FALSE_OFF", "off", 1), 0);

  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_TRUE_1", bool_val));
  EXPECT_TRUE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_TRUE_TRUE", bool_val));
  EXPECT_TRUE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_TRUE_YES", bool_val));
  EXPECT_TRUE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_TRUE_ON", bool_val));
  EXPECT_TRUE(bool_val);

  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_FALSE_0", bool_val));
  EXPECT_FALSE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_FALSE_FALSE", bool_val));
  EXPECT_FALSE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_FALSE_NO", bool_val));
  EXPECT_FALSE(bool_val);
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_FALSE_OFF", bool_val));
  EXPECT_FALSE(bool_val);
}

TEST(LLU_ENV_TEST, BoolDecoderRejectsUnsupportedValues) {
  bool bool_val = false;
  ASSERT_EQ(::setenv("LLU_ENV_TEST_BAD_BOOL", "maybe", 1), 0);

  const auto message = captureWhat([&] { static_cast<void>(llu::getenv("LLU_ENV_TEST_BAD_BOOL", bool_val)); });

  EXPECT_NE(message.find("cannot be converted to a boolean"), std::string::npos);
  EXPECT_NE(message.find("LLU_ENV_TEST_BAD_BOOL"), std::string::npos);
}

TEST(LLU_ENV_TEST, IntegerDecoderRejectsNonNumericValues) {
  long int_val = 0;
  ASSERT_EQ(::setenv("LLU_ENV_TEST_BAD_INTEGER", "12x", 1), 0);

  const auto message = captureWhat([&] { static_cast<void>(llu::getenv("LLU_ENV_TEST_BAD_INTEGER", int_val)); });

  EXPECT_NE(message.find("cannot be converted to a integer"), std::string::npos);
  EXPECT_NE(message.find("LLU_ENV_TEST_BAD_INTEGER"), std::string::npos);
}

TEST(LLU_ENV_TEST, PathDecoderSkipsEmptyColonSeparatedSegments) {
  std::vector<std::string> paths1;
  std::vector<std::string> paths2;
  std::vector<std::string> paths3;

  ASSERT_EQ(::setenv("LLU_ENV_TEST_PATHS_FILLED", ":/a:/b::/c:", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_PATHS_EMPTY", "", 1), 0);
  ASSERT_EQ(::setenv("LLU_ENV_TEST_PATHS_DELIMS", ":::::", 1), 0);

  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_PATHS_FILLED", paths1));
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_PATHS_EMPTY", paths2));
  ASSERT_TRUE(llu::getenv("LLU_ENV_TEST_PATHS_DELIMS", paths3));

  EXPECT_EQ(paths1, (std::vector<std::string>{"/a", "/b", "/c"}));
  EXPECT_TRUE(paths2.empty());
  EXPECT_TRUE(paths3.empty());
}
