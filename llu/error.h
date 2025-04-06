#ifndef LLU_ERROR_H_
#define LLU_ERROR_H_

#include <exception>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <llu/const.h>
#include <llu/macro.h>

#define LLU_ERROR(...) throw std::runtime_error(fmt::format(LLU_FILELINE() " " __VA_ARGS__))
#define LLU_PRINT(...) fmt::print("{}\n", fmt::format(__VA_ARGS__));
#define LLU_INFO(...)  fmt::print("{}{}{}\n", ::llu::kGreen, fmt::format(__VA_ARGS__), ::llu::kClear);
#define LLU_WARN(...)  fmt::print("{}{}{}\n", ::llu::kYellow, fmt::format(__VA_ARGS__), ::llu::kClear);
#define LLU_CRIT(...)  fmt::print("{}{}{}\n", ::llu::kRed, fmt::format(__VA_ARGS__), ::llu::kClear);

#define LLU_ASSERT_NOT(cond, ...) \
  if (cond) LLU_ERROR(__VA_ARGS__)
#define LLU_ASSERT(cond, ...)    LLU_ASSERT_NOT(not(cond), __VA_ARGS__)
#define LLU_ASSERT_EQ(a, b, ...) LLU_ASSERT((a) == (b), __VA_ARGS__)

#endif  //  LLU_ERROR_H_
