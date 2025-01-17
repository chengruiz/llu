#include <gtest/gtest.h>

#include <llu/error.h>

TEST(LLU_ERROR_TEST, LLU_COLOR_TEST) {
  std::cout << llu::kRed << "This is red" << std::endl;
  std::cout << llu::kGreen << "This is green" << std::endl;
  std::cout << llu::kYellow << "This is yellow" << std::endl;
  std::cout << llu::kBlue << "This is blue" << std::endl;
  std::cout << llu::kMagenta << "This is magenta" << std::endl;
  std::cout << llu::kCyan << "This is cyan" << std::endl;
  std::cout << llu::kClear << "This is white" << std::endl;
}

TEST(LLU_ERROR_TEST, LLU_PRINT_TEST) {
  LLU_INFO("This is an info message");
  LLU_WARN("This is a warning message");
  LLU_CRIT("This is a critical message");

  LLU_INFO("Info {}.", 12138);
  LLU_WARN("Warn {}?", 12138);
  LLU_CRIT("Crit {}!", 12138);
}
