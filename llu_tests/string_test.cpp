#include <gtest/gtest.h>

#include <string>

#include <llu/string.h>

TEST(LLU_STRING_TEST, NonEmptyOrReturnsTheFirstNonEmptyString) {
  EXPECT_EQ(llu::nonEmptyOr("value", "fallback"), "value");
  EXPECT_EQ(llu::nonEmptyOr("0", "fallback"), "0");
}

TEST(LLU_STRING_TEST, NonEmptyOrFallsBackWhenTheFirstStringIsEmpty) {
  EXPECT_EQ(llu::nonEmptyOr("", "fallback"), "fallback");
  EXPECT_EQ(llu::nonEmptyOr("", ""), "");
}

TEST(LLU_STRING_TEST, TrimRemovesLeadingAndTrailingWhitespace) {
  EXPECT_EQ(llu::trim("  hello world  "), "hello world");
  EXPECT_EQ(llu::trim("\t spaced value \n"), "spaced value");
  EXPECT_EQ(llu::trim("inner  spaces"), "inner  spaces");
}

TEST(LLU_STRING_TEST, TrimReturnsEmptyForWhitespaceOnlyStrings) {
  EXPECT_EQ(llu::trim(""), "");
  EXPECT_EQ(llu::trim(" \t\r\n "), "");
}

TEST(LLU_STRING_TEST, StartsWithMatchesPrefixesIncludingTheEmptyPrefix) {
  EXPECT_TRUE(llu::startsWith("prefix-value", "pre"));
  EXPECT_TRUE(llu::startsWith("prefix-value", ""));
  EXPECT_TRUE(llu::startsWith("prefix-value", "prefix-value"));
  EXPECT_FALSE(llu::startsWith("prefix-value", "value"));
  EXPECT_FALSE(llu::startsWith("short", "shorter"));
}

TEST(LLU_STRING_TEST, EndsWithMatchesSuffixesIncludingTheEmptySuffix) {
  EXPECT_TRUE(llu::endsWith("prefix-value", "value"));
  EXPECT_TRUE(llu::endsWith("prefix-value", ""));
  EXPECT_TRUE(llu::endsWith("prefix-value", "prefix-value"));
  EXPECT_FALSE(llu::endsWith("prefix-value", "prefix"));
  EXPECT_FALSE(llu::endsWith("short", "longer"));
}
