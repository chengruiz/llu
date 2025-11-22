#ifndef LLU_ENV_H_
#define LLU_ENV_H_

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

#include <fmt/core.h>

namespace llu {
inline void toLowercaseInplace(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), [](char ch) { return static_cast<char>(tolower(ch)); });
}

inline std::string toLowercase(const std::string &str) {
  std::string result = str;
  toLowercaseInplace(result);
  return result;
}

inline void toUppercaseInplace(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(), [](char ch) { return static_cast<char>(toupper(ch)); });
}

inline std::string toUppercase(const std::string &str) {
  std::string result = str;
  toUppercaseInplace(result);
  return result;
}

inline bool getenv(const char *name, std::string &result) {
  const char *var = std::getenv(name);
  if (var == nullptr) return false;
  result = var;
  return true;
}

inline bool getenv(const char *name, std::vector<std::string> &result) {
  const char *var = std::getenv(name);
  if (var == nullptr) return false;
  result.clear();
  std::string var_str(var);
  std::size_t start = 0, end = 0;
  while ((end = var_str.find(':', start)) != std::string::npos) {
    if (end > start) result.push_back(var_str.substr(start, end - start));
    start = end + 1;
  }
  std::string token = var_str.substr(start);
  if (not token.empty()) result.push_back(token);
  return true;
}

inline bool getenv(const char *name, bool &result) {
  const char *var = std::getenv(name);
  if (var == nullptr) return false;
  std::string var_str(var);
  toLowercaseInplace(var_str);
  if (var_str == "1" || var_str == "true" || var_str == "yes" || var_str == "on") {
    result = true;
  } else if (var_str == "0" || var_str == "false" || var_str == "no" || var_str == "off") {
    result = false;
  } else {
    throw std::invalid_argument(
        fmt::format("Environment Variable '{}' ({}) cannot be converted to a boolean.", name, var));
  }
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
