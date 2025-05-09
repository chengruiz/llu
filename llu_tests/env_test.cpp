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

  setenv("MY_PATHS1", ":/a:/b::/c:", 1);
  setenv("MY_PATHS2", "", 1);
  setenv("MY_PATHS3", ":::::", 1);
  std::vector<std::string> paths1;
  std::vector<std::string> paths2;
  std::vector<std::string> paths3;
  ASSERT_TRUE(llu::getenv("MY_PATHS1", paths1)) << "Expected a variable";
  ASSERT_TRUE(llu::getenv("MY_PATHS2", paths2)) << "Expected a variable";
  ASSERT_TRUE(llu::getenv("MY_PATHS3", paths3)) << "Expected a variable";
  ASSERT_EQ(paths1.size(), 3) << "Expected 4 paths";
  ASSERT_EQ(paths1[0], "/a") << "Expected /a";
  ASSERT_EQ(paths1[1], "/b") << "Expected /b";
  ASSERT_EQ(paths1[2], "/c") << "Expected /c";
  ASSERT_EQ(paths2.size(), 0) << "Expected 0 paths";
  ASSERT_EQ(paths3.size(), 0) << "Expected 5 paths";
}
