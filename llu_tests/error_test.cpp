#include <gtest/gtest.h>

#include <string>

#include <llu/error.h>

namespace {
template <typename Callback>
std::string captureStdout(Callback &&callback) {
  testing::internal::CaptureStdout();
  callback();
  return testing::internal::GetCapturedStdout();
}

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

TEST(LLU_ERROR_TEST, PrintMacrosWriteFormattedOutput) {
  EXPECT_EQ(captureStdout([] { LLU_PRINT("Value {}", 42); }), "Value 42\n");
  EXPECT_EQ(captureStdout([] { LLU_INFO("Info {}", 1); }), std::string{llu::kGreen} + "Info 1" + llu::kClear + "\n");
  EXPECT_EQ(captureStdout([] { LLU_WARN("Warn {}", 2); }), std::string{llu::kYellow} + "Warn 2" + llu::kClear + "\n");
  EXPECT_EQ(captureStdout([] { LLU_CRIT("Crit {}", 3); }), std::string{llu::kRed} + "Crit 3" + llu::kClear + "\n");
}

TEST(LLU_ERROR_TEST, ThrowMacroPrefixesMessagesWithFileContext) {
  const auto message = captureWhat([] { LLU_THROW("failure {}", 7); });

  EXPECT_NE(message.find("failure 7"), std::string::npos);
  EXPECT_NE(message.find(__FILE_NAME__), std::string::npos);
}

TEST(LLU_ERROR_TEST, AssertMacroReportsConditionAndCustomMessage) {
  const auto message = captureWhat([] { LLU_ASSERT(1 == 2, "Mismatch {}", 5); });

  EXPECT_NE(message.find("1 == 2"), std::string::npos);
  EXPECT_NE(message.find("Mismatch 5"), std::string::npos);
  EXPECT_NE(message.find(__FILE_NAME__), std::string::npos);
}

TEST(LLU_ERROR_TEST, AssertEqMacroReportsActualAndExpectedValues) {
  const auto message = captureWhat([] {
    int value = 3;
    int expected = 4;
    LLU_ASSERT_EQ(value, expected, "Values diverged");
  });

  EXPECT_NE(message.find("value == expected"), std::string::npos);
  EXPECT_NE(message.find("i.e. 3 == 4"), std::string::npos);
  EXPECT_NE(message.find("Values diverged"), std::string::npos);
}
