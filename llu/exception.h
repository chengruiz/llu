#ifndef LLU_EXCEPTION_H_
#define LLU_EXCEPTION_H_

#include <stdexcept>
#include <string>

#include <fmt/format.h>

namespace llu {
class Exception : public std::runtime_error {
 public:
  explicit Exception(std::string message) : std::runtime_error(std::move(message)) {}
  template <typename Arg1, typename... Args>
  Exception(const std::string &format, Arg1 &&arg1, Args &&...args)
      : std::runtime_error(fmt::format(format, std::forward<Arg1>(arg1), std::forward<Args>(args)...)) {}
};

struct UnderflowError final : Exception {
  using Exception::Exception;
};

struct IndexError final : Exception {
  using Exception::Exception;
};

struct NotAllocatedError final : Exception {
  using Exception::Exception;
};
}  // namespace llu

#endif  // LLU_EXCEPTION_H_
