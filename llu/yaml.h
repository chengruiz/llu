#ifndef LLU_YAML_H_
#define LLU_YAML_H_

#include <Eigen/Core>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <yaml-cpp/yaml.h>

#include <llu/error.h>
#include <llu/range.h>
#include <llu/typename.h>

template<> struct fmt::formatter<YAML::Node> : ostream_formatter {};

#define LLU_BAD_YAML(node, type) \
    LLU_ERROR("Bad conversion of Node `{}` to type `{}`.", node, ::llu::getTypeName(type))

/**
 * @brief This namespace contains utility functions for working with YAML.
 */
namespace llu::yml {
using Node = YAML::Node;

/**
 * @brief Checks if a YAML node can be converted to a given type.
 *
 * @tparam T The type to check for.
 * @param node The YAML node to check.
 * @return true if the node can be converted to the given type, false otherwise.
 */
template<typename T>
bool isType(const Node &node) {
  try {
    node.as<T>();
    return true;
  } catch (...) {
    return false;
  }
}

inline bool isBool(const Node &node) { return isType<bool>(node); }
inline bool isFloat(const Node &node) { return isType<double>(node); }

inline bool isNTuple(const Node &node, std::size_t size) {
  return node.IsSequence() and node.size() == size;
}

inline bool isValid(const Node &node) { return static_cast<bool>(node); }

template<typename Key>
bool isValid(const Node &node, const Key &key) {
  return node and not node.IsScalar() and node[key];
}

template<typename Key, typename...SeqKeys>
bool isValid(const Node &node, const Key &key, SeqKeys...seq_keys) {
  return node and not node.IsScalar() and isValid(node[key], seq_keys...);
}

inline void assertValid(const Node &node) {
  LLU_ASSERT(node, "Invalid node.");
}

template<typename Key>
void assertValid(const Node &node, const Key &key) {
  assertValid(node);
  LLU_ASSERT(node[key], "Missing Key `{}` for node `{}`.", key, node);
}

inline void assertNTuple(const Node &node, std::size_t size) {
  assertValid(node);
  LLU_ASSERT(isNTuple(node, size), "Node `{}` requires to be a {}-tuple.", node, size);
}

template<typename Key>
void assertNTuple(const Node &node, const Key &key, std::size_t size) {
  assertValid(node, key);
  LLU_ASSERT(isNTuple(node[key], size), "Value of key `{}` requires to be a {}-tuple for node `{}`.", key, size, node);
}

namespace impl {
template<typename Value>
void setTo(const Node &node, Value &value) {
  try {
    value = node.as<Value>();
  } catch (const std::runtime_error &) { /// gives a more detailed information
    LLU_BAD_YAML(node, value);
  }
}

template<typename T>
void setTo(const Node &node, std::optional<T> &value);
template<typename T>
void setTo(const Node &node, std::vector<T> &value);
template<typename T, std::size_t N>
void setTo(const Node &node, std::array<T, N> &value);
template<typename dtype>
void setTo(const Node &node, range_t<dtype> &value);
template<typename T, int N>
void setTo(const Node &node, Eigen::Matrix<T, N, 1> &value);
template<typename T, int N>
void setTo(const Node &node, Eigen::Array<T, N, 1> &value);

template<typename T>
void setTo(const Node &node, std::optional<T> &value) {
  if (node.IsNull()) {
    value.reset();
    return;
  }
  T inner_value;
  setTo(node, inner_value);
  value = inner_value;
}

template<typename T>
void setTo(const Node &node, std::vector<T> &value) {
  value.clear();
  if (node.IsScalar()) {
    value.resize(1);
    return setTo(node, value.back());
  }
  value.resize(node.size());
  for (std::size_t i{}; i < node.size(); ++i) {
    setTo(node[i], value[i]);
  }
}

template<typename T, std::size_t N>
void setTo(const Node &node, std::array<T, N> &value) {
  static_assert(N != 0, "setTo: Invalid array size 0.");
  if (node.IsScalar()) {
    T scalar;
    setTo(node, scalar);
    value.fill(scalar);
    return;
  }
  assertNTuple(node, N);
  for (std::size_t i{}; i < N; ++i) {
    setTo(node[i], value[i]);
  }
}

template<typename dtype>
void setTo(const Node &node, range_t<dtype> &value) {
  if (node.IsScalar()) {
    setTo(node, value.lower());
    value.upper() = value.lower();
  } else if (node.IsMap()) {
    setTo(node["lower"], value.lower());
    setTo(node["upper"], value.upper());
  } else if (isNTuple(node, 2)) {
    setTo(node[0], value.lower());
    setTo(node[1], value.upper());
  } else {
    LLU_BAD_YAML(node, value);
  }
}

template<typename T, int N>
void setTo(const Node &node, Eigen::Matrix<T, N, 1> &value) {
  if constexpr (N == -1) {
    std::vector<T> result;
    setTo(node, result);
    value = Eigen::Map<Eigen::Matrix<T, -1, 1>>(result.data(), result.size());
  } else {
    std::array<T, N> result;
    setTo(node, result);
    value = Eigen::Map<Eigen::Matrix<T, N, 1>>(result.data());
  }
}

template<typename T, int N>
void setTo(const Node &node, Eigen::Array<T, N, 1> &value) {
  if constexpr (N == -1) {
    std::vector<T> result;
    setTo(node, result);
    value = Eigen::Map<Eigen::Matrix<T, -1, 1>>(result.data(), result.size());
  } else {
    std::array<T, N> result;
    setTo(node, result);
    value = Eigen::Map<Eigen::Matrix<T, N, 1>>(result.data());
  }
}
} // namespace impl

template<typename Value>
void setTo(const Node &node, Value &value) {
  assertValid(node);
  impl::setTo(node, value);
}

template<typename Value, typename Key>
void setTo(const Node &node, const Key &key, Value &value) {
  assertValid(node, key);
  impl::setTo(node[key], value);
}

template<typename Value>
void setIf(const Node &node, Value &value) {
  if (node) impl::setTo(node, value);
}

template<typename Value, typename Key>
void setIf(const Node &node, const Key &key, Value &value) {
  if (isValid(node, key)) impl::setTo(node[key], value);
}

template<typename Key>
Node getItem(const Node &node, const Key &key) {
  assertValid(node, key);
  return node[key];
}

template<typename Value>
Value readAs(const Node &node) {
  Value value;
  setTo(node, value);
  return value;
}

template<typename Value, typename Key>
Value readAs(const Node &node, const Key &key) {
  Value value;
  setTo(node, key, value);
  return value;
}

template<typename Value>
Value readIf(const Node &node, const Value &default_value) {
  return node ? readAs<Value>(node) : default_value;
}

template<typename Value, typename Key>
Value readIf(const Node &node, const Key &key, const Value &default_value) {
  return node ? readIf<Value>(node[key], default_value) : default_value;
}
} // namespace llu::yml

#undef LLU_BAD_YAML

#endif  // LLU_YAML_H_
