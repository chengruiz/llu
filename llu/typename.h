#ifndef LLU_TYPE_H_
#define LLU_TYPE_H_

#include <cxxabi.h>
#include <memory>
#include <string>

namespace llu {
template<typename T>
std::string getTypeName() {
  std::unique_ptr<char, decltype(std::free) *> type_name{
    abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr), std::free
  };
  return type_name.get();
}

template<typename T>
std::string getTypeName(const T &v) {
  std::unique_ptr<char, decltype(std::free) *> type_name{
    abi::__cxa_demangle(typeid(v).name(), nullptr, nullptr, nullptr), std::free
  };
  return type_name.get();
}
} // namespace llu

#endif  // LLU_TYPE_H_
