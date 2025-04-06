#ifndef LLU_CONST_H_
#define LLU_CONST_H_

namespace llu {
constexpr const char *kClear   = "\033[0m";
constexpr const char *kRed     = "\033[1;31m";
constexpr const char *kGreen   = "\033[1;32m";
constexpr const char *kYellow  = "\033[1;33m";
constexpr const char *kBlue    = "\033[1;34m";
constexpr const char *kMagenta = "\033[1;35m";
constexpr const char *kCyan    = "\033[1;36m";

constexpr double kEPS = 1e-8;
}  // namespace llu

#endif  // LLU_CONST_H_
