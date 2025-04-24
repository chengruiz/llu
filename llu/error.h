#ifndef LLU_ERROR_H_
#define LLU_ERROR_H_

#include <stdexcept>

#include <fmt/chrono.h>
#include <fmt/core.h>

#include <llu/const.h>
#include <llu/macro.h>

#define LLU_PRINT(...) fmt::print("{}\n", fmt::format(__VA_ARGS__));
#define LLU_INFO(...)  fmt::print("{}{}{}\n", ::llu::kGreen, fmt::format(__VA_ARGS__), ::llu::kClear);
#define LLU_WARN(...)  fmt::print("{}{}{}\n", ::llu::kYellow, fmt::format(__VA_ARGS__), ::llu::kClear);
#define LLU_CRIT(...)  fmt::print("{}{}{}\n", ::llu::kRed, fmt::format(__VA_ARGS__), ::llu::kClear);

namespace llu {
namespace impl {
template <typename... Args>
[[noreturn]] void errorHelper(const char *file_line, const char *msg, Args &&...args) {
  std::string prefix = fmt::format("{} ", file_line);
  throw std::runtime_error(prefix + fmt::format(msg, std::forward<Args>(args)...));
}

template <typename T, typename... Args>
void assertHelper(const T &condition, const char *condition_str, const char *file_line, const char *msg,
                  Args &&...args) {
  if (condition) return;
  std::string prefix = fmt::format("{} Assertion {}{}{} failed: ", file_line, kUnderline, condition_str, kClear);
  throw std::runtime_error(prefix + fmt::format(msg, std::forward<Args>(args)...));
}

template <typename T1, typename T2, typename... Args>
void assertEqHelper(T1 value, T2 expected, const char *value_str, const char *expected_str, const char *file_line,
                    const char *msg, Args &&...args) {
  if (value == expected) return;
  std::string prefix = fmt::format("{} Assertion {}{} == {}{} (i.e. {} == {}) failed: ", file_line, kUnderline,
                                   value_str, expected_str, kClear, value, expected);
  throw std::runtime_error(prefix + fmt::format(msg, std::forward<Args>(args)...));
}
}  // namespace impl
}  // namespace llu

#define LLU_ERROR(...)             ::llu::impl::errorHelper(LLU_FILELINE(), __VA_ARGS__)
#define LLU_ASSERT(condition, ...) ::llu::impl::assertHelper(condition, #condition, LLU_FILELINE(), __VA_ARGS__)
#define LLU_ASSERT_EQ(value, expected, ...) \
  ::llu::impl::assertEqHelper(value, expected, #value, #expected, LLU_FILELINE(), __VA_ARGS__)
#endif  //  LLU_ERROR_H_
