#include <gtest/gtest.h>

#include <llu/env.h>

TEST(LLU_ENV_TEST, LLU_ENV_TEST) {
  std::string str_val;
  long int_val;

  setenv("A_VAR", "123", 1);

  ASSERT_FALSE(llu::getenv("NOT_A_VAR", str_val)) << "Expected not a variable";
  ASSERT_FALSE(llu::getenv("NOT_A_VAR", int_val)) << "Expected not a variable";

  ASSERT_TRUE(llu::getenv("HOME", str_val)) << "Expected a variable";
  ASSERT_TRUE(llu::getenv("A_VAR", int_val)) << "Expected a variable";
  ASSERT_EQ(int_val, 123) << "Expected 123";

  try {
    llu::getenv("HOME", int_val);
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::exception &e) {}
}
