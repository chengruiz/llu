#include <gtest/gtest.h>

#include <llu/const.h>

TEST(LLU_CONST_TEST, LLU_COLOR_TEST) {
  std::cout << llu::kRed << "This is red" << std::endl;
  std::cout << llu::kGreen << "This is green" << std::endl;
  std::cout << llu::kYellow << "This is yellow" << std::endl;
  std::cout << llu::kBlue << "This is blue" << std::endl;
  std::cout << llu::kMagenta << "This is magenta" << std::endl;
  std::cout << llu::kCyan << "This is cyan" << std::endl;
  std::cout << llu::kClear << "This is white" << std::endl;
}
