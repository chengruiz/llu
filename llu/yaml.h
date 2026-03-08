#ifndef LLU_YAML_H_
#define LLU_YAML_H_

#include <array>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#if FMT_VERSION < 90000
#include <sstream>
#endif

#if __cplusplus >= 201703L
#include <optional>
#endif

#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <yaml-cpp/yaml.h>
#include <Eigen/Core>

#include <llu/error.h>
#include <llu/range.h>
#include <llu/typename.h>

#if FMT_VERSION >= 90000
template <>
struct fmt::formatter<YAML::Node> : fmt::ostream_formatter {};
#endif

/**
 * @brief This namespace contains utility functions for working with YAML.
 */
namespace llu {
namespace yml {
using Node = YAML::Node;

inline std::string formatNode(const Node &node) {
#if FMT_VERSION < 90000
  std::stringstream ss;
  ss << node;
  std::string node_str = ss.str();
#else
  std::string node_str = fmt::format("'{}'", node);
#endif
  if (node_str.find('\n') != std::string::npos) {
    for (std::size_t pos = 0; (pos = node_str.find('\n', pos)) != std::string::npos; pos += 3) {
      node_str.replace(pos, 1, "\n  ");
    }
    node_str = fmt::format("'''\n  {}\n'''", node_str);
  }
  return node_str;
}

class ParsingError : public std::exception {
 public:
  explicit ParsingError(YAML::Node node, std::string reason, std::string access_path = "")
      : context_node_(std::move(node)), reason_(std::move(reason)), access_path_(std::move(access_path)) {}

  template <typename Key>
  ParsingError withParent(const Node &parent_node, const Key &key) const {
    return ParsingError(parent_node, reason_, fmt::format("[{}]{}", key, access_path_));
  }

  const char *what() const noexcept override {
    if (not what_.empty()) return what_.c_str();
    try {
      what_ = fmt::format("Error parsing YAML: {}.", reason_);
      if (context_node_) what_ += fmt::format("\nNode: {}", formatNode(context_node_));
      if (not access_path_.empty()) what_ += fmt::format("\nPath: {}", access_path_);
    } catch (...) {
      what_ = reason_;
    }
    return what_.c_str();
  }

 private:
  YAML::Node context_node_;
  std::string reason_;
  std::string access_path_;
  mutable std::string what_;
};

struct BadConversionError : ParsingError {
  template <typename T>
  BadConversionError(const YAML::Node &node, const T &value)
      : ParsingError(node, fmt::format("Bad conversion to type '{}'.", getTypeName(value))) {}
};

inline Node loadFile(const std::string &path) {
  try {
    return YAML::LoadFile(path);
  } catch (const YAML::BadFile &error) {
    throw std::runtime_error(fmt::format("Failed to load YAML file '{}' ({}).", path, error.what()));
  }
}

inline Node loadFileIf(const std::string &path) {
  try {
    return YAML::LoadFile(path);
  } catch (const YAML::BadFile &) {
    return {};
  }
}

/**
 * @brief Checks if a YAML node can be converted to a given type.
 *
 * @tparam T The type to check for.
 * @param node The YAML node to check.
 * @return true if the node can be converted to the given type, false otherwise.
 */
template <typename T>
bool isType(const Node &node) {
  try {
    node.as<T>();
    return true;
  } catch (const YAML::Exception &) {
    return false;
  }
}

inline bool isBool(const Node &node) { return isType<bool>(node); }
inline bool isFloat(const Node &node) { return isType<double>(node); }
inline bool isDefined(const Node &node) { return node.IsDefined(); }
inline bool hasValue(const Node &node) { return node and not node.IsNull(); }

template <typename Key>
bool isDefined(const Node &node, const Key &key) {
  return node and not node.IsScalar() and node[key].IsDefined();
}

template <typename Key, typename... Keys>
bool isDefined(const Node &node, const Key &key, const Keys &...keys) {
  return isDefined(node, key) and isDefined(node[key], keys...);
}

template <typename Key>
bool hasValue(const Node &node, const Key &key) {
  return isDefined(node, key) and not node[key].IsNull();
}

template <typename Key, typename... Keys>
bool hasValue(const Node &node, const Key &key, const Keys &...keys) {
  return hasValue(node, key) and hasValue(node[key], keys...);
}

inline void assertDefined(const Node &node) {
  if (not isDefined(node)) throw ParsingError(node, "Undefined node");
}

template <typename Key>
void assertDefined(const Node &node, const Key &key) {
  if (isDefined(node, key)) return;
  throw ParsingError(node, "Undefined node", key);
}

template <typename Key, typename... Keys>
void assertDefined(const Node &node, const Key &key, const Keys &...keys) {
  assertDefined(node, key);
  try {
    assertDefined(node[key], keys...);
  } catch (const ParsingError &error) {
    throw error.withParent(node, key);
  }
}

inline std::string getDefinedKey(const Node &node, const std::string &key1, const std::string &key2) {
  if (not node.IsMap()) throw ParsingError(node, "Expected a map node");
  if (isDefined(node, key1)) return key1;
  if (isDefined(node, key2)) return key2;
  return "";
}

inline std::string getDefinedKey(const Node &node, const std::string &key1, const std::string &key2,
                                 const std::string &key3) {
  if (not node.IsMap()) throw ParsingError(node, "Expected a map node");
  if (isDefined(node, key1)) return key1;
  if (isDefined(node, key2)) return key2;
  if (isDefined(node, key3)) return key3;
  return "";
}

inline std::string getDefinedKey(const Node &node, const std::string &key1, const std::string &key2,
                                 const std::string &key3, const std::string &key4) {
  if (not node.IsMap()) throw ParsingError(node, "Expected a map node");
  if (isDefined(node, key1)) return key1;
  if (isDefined(node, key2)) return key2;
  if (isDefined(node, key3)) return key3;
  if (isDefined(node, key4)) return key4;
  return "";
}

inline void assertHasValue(const Node &node) {
  assertDefined(node);
  if (node.IsNull()) throw ParsingError(node, "Expected a value");
}

template <typename Key>
void assertHasValue(const Node &node, const Key &key) {
  assertDefined(node, key);
  if (not node[key].IsNull()) return;
  throw ParsingError(node, "Expected a value", key);
}

template <typename Key, typename... Keys>
void assertHasValue(const Node &node, const Key &key, const Keys &...keys) {
  assertHasValue(node, key);
  try {
    assertHasValue(node[key], keys...);
  } catch (const ParsingError &error) {
    throw error.withParent(node, key);
  }
}

inline void assertSequence(const Node &node) {
  assertHasValue(node);
  if (node.IsSequence()) return;
  throw ParsingError(node, "Expected a sequence");
}

template <typename Key>
inline void assertSequence(const Node &node, const Key &key) {
  if (node and node[key].IsSequence()) return;
  throw ParsingError(node, "Expected a sequence", key);
}

inline bool isSequenceOfSize(const Node &node, std::size_t size) { return node.IsSequence() and node.size() == size; }

inline void assertSequenceOfSize(const Node &node, std::size_t size) {
  assertHasValue(node);
  if (isSequenceOfSize(node, size)) return;
  throw ParsingError(node, fmt::format("Expected a sequence of size {}", size));
}

template <typename Key>
void assertSequenceOfSize(const Node &node, const Key &key, std::size_t size) {
  assertHasValue(node, key);
  if (isSequenceOfSize(node[key], size)) return;
  throw ParsingError(node, fmt::format("Expected a sequence of size {}", size), key);
}

namespace impl {
template <typename Value>
void setTo(const Node &node, Value &value) {
  try {
    value = node.as<Value>();
  } catch (const YAML::Exception &) {  // Gives detailed information
    throw BadConversionError(node, value);
  }
}

inline void setTo(const Node &node, std::vector<bool>::reference value) {
  bool scalar;
  setTo(node, scalar);
  value = scalar;
}

template <typename T>
void setTo(const Node &node, std::vector<T> &value);
template <typename T, std::size_t N>
void setTo(const Node &node, std::array<T, N> &value);
template <typename dtype>
void setTo(const Node &node, range_t<dtype> &value);
template <typename T, int N>
void setTo(const Node &node, Eigen::Matrix<T, N, 1> &value);
template <typename T>
void setTo(const Node &node, Eigen::Matrix<T, -1, 1> &value);
template <typename T, int N>
void setTo(const Node &node, Eigen::Array<T, N, 1> &value);
template <typename T>
void setTo(const Node &node, Eigen::Array<T, -1, 1> &value);

template <typename T>
void setTo(const Node &node, std::vector<T> &value) {
  if (node.IsScalar()) {
    if (value.empty()) value.resize(1);
    setTo(node, value.front());
    for (std::size_t i{1}; i < value.size(); ++i) {
      value[i] = value.front();
    }
    return;
  }

  if (not node.IsSequence()) {
    throw ParsingError(node, "Expected scalar or sequence");
  }
  if (value.empty()) {
    value.resize(node.size());
  } else if (node.size() != value.size()) {
    throw ParsingError(node, fmt::format("Expected size {}, but got {}", value.size(), node.size()));
  }
  for (std::size_t i{}; i < node.size(); ++i) {
    try {
      setTo(node[i], value[i]);
    } catch (const ParsingError &error) {
      throw error.withParent(node, i);
    }
  }
}

template <typename T, std::size_t N>
void setTo(const Node &node, std::array<T, N> &value) {
  static_assert(N != 0, "setTo: Invalid array size (0).");
  if (node.IsScalar()) {
    T scalar;
    setTo(node, scalar);
    value.fill(scalar);
    return;
  }

  assertSequenceOfSize(node, N);
  for (std::size_t i{}; i < N; ++i) {
    try {
      setTo(node[i], value[i]);
    } catch (const ParsingError &error) {
      throw error.withParent(node, i);
    }
  }
}

template <typename dtype>
void setTo(const Node &node, range_t<dtype> &value) {
  if (node.IsScalar()) {
    setTo(node, value.lower());
    value.upper() = value.lower();
  } else if (node.IsMap()) {
    assertHasValue(node, "lower");
    assertHasValue(node, "upper");
    try {
      setTo(node["lower"], value.lower());
    } catch (const ParsingError &error) {
      throw error.withParent(node, "lower");
    }
    try {
      setTo(node["upper"], value.upper());
    } catch (const ParsingError &error) {
      throw error.withParent(node, "upper");
    }
  } else if (isSequenceOfSize(node, 2)) {
    try {
      setTo(node[0], value.lower());
    } catch (const ParsingError &error) {
      throw error.withParent(node, 0);
    }
    try {
      setTo(node[1], value.upper());
    } catch (const ParsingError &error) {
      throw error.withParent(node, 1);
    }
  } else {
    throw BadConversionError(node, value);
  }
}

template <typename T, int N>
void setTo(const Node &node, Eigen::Matrix<T, N, 1> &value) {
  std::array<T, N> result;
  setTo(node, result);
  value = Eigen::Map<Eigen::Matrix<T, N, 1>>(result.data());
}

template <typename T>
void setTo(const Node &node, Eigen::Matrix<T, -1, 1> &value) {
  std::vector<T> result;
  if (value.size() != 0) result.resize(value.size());
  setTo(node, result);
  value = Eigen::Map<Eigen::Matrix<T, -1, 1>>(result.data(), result.size());
}

template <typename T, int N>
void setTo(const Node &node, Eigen::Array<T, N, 1> &value) {
  std::array<T, N> result;
  setTo(node, result);
  value = Eigen::Map<Eigen::Array<T, N, 1>>(result.data());
}

template <typename T>
void setTo(const Node &node, Eigen::Array<T, -1, 1> &value) {
  std::vector<T> result;
  if (value.size() != 0) result.resize(value.size());
  setTo(node, result);
  value = Eigen::Map<Eigen::Array<T, -1, 1>>(result.data(), result.size());
}

#if __cplusplus >= 201703L
template <typename T>
void setTo(const Node &node, std::optional<T> &value) {
  if (node.IsNull()) {
    value.reset();
    return;
  }
  T inner_value;
  setTo(node, inner_value);
  value = inner_value;
}
#endif
}  // namespace impl

template <typename Value>
void setTo(const Node &node, Value &value) {
  assertDefined(node);
  impl::setTo(node, value);
}

template <typename Value, typename Key>
void setTo(const Node &node, const Key &key, Value &value) {
  assertDefined(node, key);
  try {
    impl::setTo(node[key], value);
  } catch (const ParsingError &error) {
    throw error.withParent(node, key);
  }
}

template <typename Value>
void setIf(const Node &node, Value &value) {
  if (isDefined(node)) impl::setTo(node, value);
}

template <typename Value, typename Key>
void setIf(const Node &node, const Key &key, Value &value) {
  if (isDefined(node, key)) {
    try {
      impl::setTo(node[key], value);
    } catch (const ParsingError &error) {
      throw error.withParent(node, key);
    }
  }
}

template <typename Key>
Node getItem(const Node &node, const Key &key) {
  assertDefined(node, key);
  return node[key];
}

template <typename Value>
Value readAs(const Node &node) {
  Value value;
  setTo(node, value);
  return value;
}

template <typename Value, typename Key>
Value readAs(const Node &node, const Key &key) {
  Value value;
  setTo(node, key, value);
  return value;
}

template <typename Value>
Value readIf(const Node &node, const Value &default_value) {
  return isDefined(node) ? readAs<Value>(node) : default_value;
}

template <typename Value, typename Key>
Value readIf(const Node &node, const Key &key, const Value &default_value) {
  return isDefined(node, key) ? readAs<Value>(node, key) : default_value;
}
}  // namespace yml
}  // namespace llu

#endif  // LLU_YAML_H_
