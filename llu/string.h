#ifndef LLU_STRING_H_
#define LLU_STRING_H_

#include <algorithm>
#include <cctype>
#include <string>

namespace llu {
inline std::string nonEmptyOr(const std::string &str1, const std::string &str2) { return str1.empty() ? str2 : str1; }

inline void toLowercaseInplace(std::string &value) {
  std::transform(value.begin(), value.end(), value.begin(), [](char ch) { return static_cast<char>(tolower(ch)); });
}

inline std::string toLowercase(std::string value) {
  toLowercaseInplace(value);
  return value;
}

inline void toUppercaseInplace(std::string &value) {
  std::transform(value.begin(), value.end(), value.begin(), [](char ch) { return static_cast<char>(toupper(ch)); });
}

inline std::string toUppercase(std::string value) {
  toUppercaseInplace(value);
  return value;
}

inline std::string trim(const std::string &value) {
  std::size_t first = 0;
  while (first < value.size() and std::isspace(static_cast<unsigned char>(value[first]))) {
    ++first;
  }

  std::size_t last = value.size();
  while (last > first and std::isspace(static_cast<unsigned char>(value[last - 1]))) {
    --last;
  }
  return value.substr(first, last - first);
}

inline bool startsWith(const std::string &str, const std::string &prefix) {
  return str.size() >= prefix.size() and str.compare(0, prefix.size(), prefix) == 0;
}

inline bool endsWith(const std::string &str, const std::string &suffix) {
  return str.size() >= suffix.size() and str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
}  // namespace llu

#endif  // LLU_STRING_H_
