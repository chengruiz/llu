#ifndef LLU_ENV_H_
#define LLU_ENV_H_

#include <cstdlib>
#include <string>

#include <fmt/core.h>

namespace llu {
inline bool getenv(const char *name, std::string &result) {
  const char *var = std::getenv(name);
  if (var == nullptr) return false;
  result = var;
  return true;
}

inline bool getenv(const char *name, long &result) {
  const char *var = std::getenv(name);
  if (var == nullptr) return false;
  char *end{nullptr};
  long value = std::strtol(var, &end, 10);
  if (*end != '\0') {
    throw std::invalid_argument(
        fmt::format("Environment Variable '{}' ({}) cannot be converted to a integer.", name, var));
  }

  result = value;
  return true;
}
}  // namespace llu

#endif  // LLU_ENV_H_
