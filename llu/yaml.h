#ifndef LLU_YAML_H_
#define LLU_YAML_H_

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
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

#include <fmt/core.h>
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
struct YamlError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct BadFileError : YamlError {
  using YamlError::YamlError;
};

inline std::string toString(const YAML::Node &node) {
#if FMT_VERSION < 90000
  std::stringstream ss;
  ss << node;
  std::string node_str = ss.str();
#else
  std::string node_str = fmt::format("{}", node);
#endif
  if (node_str.find('\n') != std::string::npos) {
    for (std::size_t pos = 0; (pos = node_str.find('\n', pos)) != std::string::npos; pos += 3) {
      node_str.replace(pos, 1, "\n  ");
    }
    node_str = fmt::format("''\n  {}\n''", node_str);
  }
  return fmt::format("'{}'", node_str);
}

template <typename T>
struct Decoder {
  static void decode(const YAML::Node &node, T &value) {
    try {
      value = node.as<T>();
    } catch (const YAML::Exception &) {
      throw YamlError(fmt::format("Failed to decode as {}", getTypeName(value)));
    }
  }
};

class Node {
 public:
  class iterator_value;
  class iterator;
  class const_iterator;

  struct Context {
    std::string filename;
    YAML::Node top_node;
    std::vector<YAML::Node> keys;
  };

  Node() : node_(YAML::Node(YAML::NodeType::Undefined)) {}
  explicit Node(YAML::Node node) : node_(std::move(node)) {
    if (node_) context_.top_node = node_;
  }
  Node(YAML::Node node, Context context) : node_(std::move(node)), context_(std::move(context)) {
    if (not context_.top_node and node_) context_.top_node = node_;
  }

  operator YAML::Node() const { return node_; }
  YAML::Node &native() { return node_; }
  const YAML::Node &native() const { return node_; }
  const Context &context() const { return context_; }
  std::size_t size() const { return isDefined() ? node_.size() : 0; }
  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  template <typename T>
  bool isType() const;
  bool isBool() const { return isType<bool>(); }
  bool isFloat() const { return isType<double>(); }
  bool isDefined() const { return node_.IsDefined(); }
  bool isSequence() const { return isDefined() and node_.IsSequence(); }
  bool isSequence(std::size_t size) const { return isSequence() and node_.size() == size; }
  bool isScalar() const { return isDefined() and node_.IsScalar(); }
  bool isMap() const { return isDefined() and node_.IsMap(); }
  bool isNull() const { return isDefined() and node_.IsNull(); }
  bool hasValue() const { return isDefined() and not isNull(); }

  inline void assertBool() const { throwUnless(isBool(), "Expected a boolean value"); }
  inline void assertFloat() const { throwUnless(isFloat(), "Expected a floating-point value"); }
  inline void assertDefined() const { throwUnless(isDefined(), "Undefined node"); }
  inline void assertSequence() const { throwUnless(isSequence(), "Expected a sequence"); }
  inline void assertNonEmptySequence() const {
    throwUnless(isSequence() and size() > 0, "Expected a non-empty sequence");
  }
  inline void assertSequence(std::size_t size) const {
    throwUnless(isSequence(size), fmt::format("Expected a sequence of size {}", size));
  }
  inline void assertScalar() const { throwUnless(isScalar(), "Expected a scalar"); }
  inline void assertMap() const { throwUnless(isMap(), "Expected a map"); }
  inline void assertIterable() const { throwUnless(isSequence() or isMap(), "Expected a sequence or map"); }
  inline void assertHasValue() const { throwUnless(hasValue(), "Expected a value"); }
  template <typename Key, typename... Keys>
  inline void assertHasValue(const Key &key, const Keys &...keys) const {
    throwUnless(operator[](key).hasValue(), fmt::format("Expected key '{}' to have a value", key));
    assertHasValue(keys...);
  }
  void assertMutuallyExclusive(std::initializer_list<std::string> keys) const;
  std::string getDefinedKey(std::initializer_list<std::string> keys) const;

  template <typename T>
  T as() const;
  template <typename T>
  T as(const T &default_value) const;
  template <typename T>
  void to(T &value, bool allow_missing = false) const;
  template <typename Key>
  Node operator[](const Key &key) const;

  [[noreturn]] void throwError(const std::string &reason) const;
  void throwUnless(bool condition, const std::string &reason) const { throwIf(not condition, reason); }
  void throwIf(bool condition, const std::string &reason) const {
    if (condition) throwError(reason);
  }

 private:
  template <typename Key>
  Context makeChildContext(const Key &key) const {
    Context child_context = context_;
    child_context.keys.emplace_back(key);
    return child_context;
  }

  YAML::Node node_;
  Context context_;
};

template <typename T>
bool Node::isType() const {
  try {
    T value{};
    Decoder<T>::decode(node_, value);
    return true;
  } catch (const std::exception &) {
    return false;
  }
}

inline void Node::assertMutuallyExclusive(std::initializer_list<std::string> keys) const {
  assertMap();
  std::string found_key;
  for (const auto &key : keys) {
    if (operator[](key).hasValue()) {
      throwIf(not found_key.empty(), fmt::format("Mutually exclusive keys {}", keys));
      found_key = key;
    }
  }
}

inline std::string Node::getDefinedKey(std::initializer_list<std::string> keys) const {
  assertMap();
  for (const auto &key : keys) {
    if (operator[](key).hasValue()) return key;
  }
  return "";
}

template <typename T>
T Node::as() const {
  T result;
  to(result);
  return result;
}

template <typename T>
T Node::as(const T &default_value) const {
  return (isDefined() and not isNull()) ? as<T>() : default_value;
}

template <typename T>
void Node::to(T &value, bool allow_missing) const {
  if (allow_missing and (not isDefined() or isNull())) return;
  throwIf(not isDefined(), "Undefined node");
  try {
    Decoder<T>::decode(node_, value);
  } catch (const YamlError &error) {
    throwIf(true, error.what());
  }
}

template <typename Key>
Node Node::operator[](const Key &key) const {
  throwIf(isScalar(), fmt::format("Cannot access key '{}' on a scalar node", key));
  // Enable chained access to nested nodes even if some intermediate nodes are missing or null
  if (not hasValue()) return Node(YAML::Node(YAML::NodeType::Undefined), makeChildContext(key));
  return Node(node_[key], makeChildContext(key));
}

inline void Node::throwError(const std::string &reason) const {
  std::string what = fmt::format("YamlError: {}.", reason);
  if (node_) what += fmt::format("\nNode: {}", toString(node_));
  if (not context_.filename.empty()) {
    what += fmt::format("\nFile: {}.", context_.filename);
  } else if (context_.top_node) {
    what += fmt::format("\nTop-level node: {}", toString(context_.top_node));
  }
  if (not context_.keys.empty()) {
    what += "\nFull path: ";
    for (const auto &key : context_.keys) {
      what += fmt::format("[{}]", key);
    }
  }
  throw YamlError(what);
}

class Node::iterator_value : public Node, public std::pair<Node, Node> {
 public:
  iterator_value() = default;
  explicit iterator_value(Node node) : Node(std::move(node)), std::pair<Node, Node>(Node{}, Node{}) {}
  explicit iterator_value(Node key, Node value) : Node(), std::pair<Node, Node>(std::move(key), std::move(value)) {}
};

class Node::iterator {
 private:
  struct proxy {
    explicit proxy(iterator_value value) : value(std::move(value)) {}
    iterator_value *operator->() { return std::addressof(value); }
    iterator_value value;
  };

 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = iterator_value;
  using difference_type   = std::ptrdiff_t;
  using pointer           = value_type *;
  using reference         = value_type;

  iterator() = default;
  iterator(Node *parent, YAML::iterator it, std::size_t index = 0)
      : parent_(parent), it_(std::move(it)), index_(index) {}

  iterator &operator++() {
    ++it_;
    ++index_;
    return *this;
  }
  iterator operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const iterator &lhs, const iterator &rhs) {
    return lhs.parent_ == rhs.parent_ and lhs.it_ == rhs.it_;
  }
  friend bool operator!=(const iterator &lhs, const iterator &rhs) { return !(lhs == rhs); }

  value_type operator*() const {
    const auto value = *it_;
    if (value.first.IsDefined() and value.second.IsDefined()) {
      Node key(value.first, parent_->makeChildContext(value.first));
      return value_type(std::move(key), Node(value.second, parent_->makeChildContext(value.first)));
    }
    return value_type(Node(YAML::Node(value), parent_->makeChildContext(index_)));
  }
  proxy operator->() const { return proxy(**this); }

  friend class const_iterator;

 private:
  Node *parent_ = nullptr;
  YAML::iterator it_;
  std::size_t index_ = 0;
};

class Node::const_iterator {
 private:
  struct proxy {
    explicit proxy(iterator_value value) : value(std::move(value)) {}
    const iterator_value *operator->() const { return std::addressof(value); }
    iterator_value value;
  };

 public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = iterator_value;
  using difference_type   = std::ptrdiff_t;
  using pointer           = const value_type *;
  using reference         = value_type;

  const_iterator() = default;
  const_iterator(const Node *parent, YAML::const_iterator it, std::size_t index = 0)
      : parent_(parent), it_(std::move(it)), index_(index) {}
  const_iterator(const iterator &other) : parent_(other.parent_), it_(other.it_), index_(other.index_) {}

  const_iterator &operator++() {
    ++it_;
    ++index_;
    return *this;
  }
  const_iterator operator++(int) {
    const_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const const_iterator &lhs, const const_iterator &rhs) {
    return lhs.parent_ == rhs.parent_ and lhs.it_ == rhs.it_;
  }
  friend bool operator!=(const const_iterator &lhs, const const_iterator &rhs) { return !(lhs == rhs); }

  value_type operator*() const {
    const auto value = *it_;
    if (value.first.IsDefined() and value.second.IsDefined()) {
      Node key(value.first, parent_->makeChildContext(value.first));
      return value_type(std::move(key), Node(value.second, parent_->makeChildContext(value.first)));
    }
    return value_type(Node(YAML::Node(value), parent_->makeChildContext(index_)));
  }
  proxy operator->() const { return proxy(**this); }

 private:
  const Node *parent_ = nullptr;
  YAML::const_iterator it_;
  std::size_t index_ = 0;
};

inline Node::iterator Node::begin() { return iterator(this, node_.begin()); }
inline Node::const_iterator Node::begin() const { return const_iterator(this, node_.begin()); }
inline Node::iterator Node::end() { return iterator(this, node_.end(), size()); }
inline Node::const_iterator Node::end() const { return const_iterator(this, node_.end(), size()); }
inline Node::const_iterator Node::cbegin() const { return begin(); }
inline Node::const_iterator Node::cend() const { return end(); }

inline Node loadFile(const std::string &path, bool allow_missing = false) {
  try {
    YAML::Node node = YAML::LoadFile(path);
    Node::Context context;
    context.filename = path;
    context.top_node = node;
    return Node(std::move(node), std::move(context));
  } catch (const YAML::BadFile &error) {
    if (allow_missing) {
      Node::Context context;
      context.filename = path;
      return Node(YAML::Node(YAML::NodeType::Undefined), std::move(context));
    }
    throw BadFileError(fmt::format("Failed to load YAML file '{}': {}.", path, error.what()));
  }
}

template <typename T>
struct Decoder<std::vector<T>> {
  static void decode(const YAML::Node &node, std::vector<T> &value) {
    if (node.IsScalar()) {
      if (value.empty()) value.resize(1);
      Decoder<T>::decode(node, value.front());
      for (std::size_t i{1}; i < value.size(); ++i) {
        value[i] = value.front();
      }
      return;
    }

    if (not node.IsSequence()) throw YamlError("Expected scalar or sequence");
    if (value.empty()) {
      value.resize(node.size());
    } else if (node.size() != value.size()) {
      throw YamlError(fmt::format("Expected size {}, but got {}", value.size(), node.size()));
    }

    for (std::size_t i{}; i < node.size(); ++i) {
      try {
        Decoder<T>::decode(node[i], value[i]);
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} at index {}.", error.what(), i));
      }
    }
  }
};

template <>
struct Decoder<std::vector<bool>> {
  static void decode(const YAML::Node &node, std::vector<bool> &value) {
    if (node.IsScalar()) {
      if (value.empty()) value.resize(1);
      bool scalar{};
      Decoder<bool>::decode(node, scalar);
      for (std::size_t i{}; i < value.size(); ++i) {
        value[i] = scalar;
      }
      return;
    }

    if (not node.IsSequence()) throw YamlError("Expected scalar or sequence");
    if (value.empty()) {
      value.resize(node.size());
    } else if (node.size() != value.size()) {
      throw YamlError(fmt::format("Expected size {}, but got {}", value.size(), node.size()));
    }

    for (std::size_t i{}; i < node.size(); ++i) {
      try {
        bool scalar{};
        Decoder<bool>::decode(node[i], scalar);
        value[i] = scalar;
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} at index {}.", error.what(), i));
      }
    }
  }
};

template <typename T, std::size_t N>
struct Decoder<std::array<T, N>> {
  static void decode(const YAML::Node &node, std::array<T, N> &value) {
    if (node.IsScalar()) {
      T scalar{};
      Decoder<T>::decode(node, scalar);
      value.fill(scalar);
      return;
    }
    if (not node.IsSequence()) throw YamlError("Expected scalar or sequence");
    if (node.size() != N) {
      throw YamlError(fmt::format("Expected size {}, but got {}", N, node.size()));
    }

    for (std::size_t i{}; i < N; ++i) {
      try {
        Decoder<T>::decode(node[i], value[i]);
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} at index {}.", error.what(), i));
      }
    }
  }
};

template <typename T>
struct Decoder<range_t<T>> {
  static void decode(const YAML::Node &node, range_t<T> &value) {
    if (node.IsScalar()) {
      Decoder<T>::decode(node, value.lower());
      value.upper() = value.lower();
    } else if (node.IsMap()) {
      try {
        Decoder<T>::decode(node["lower"], value.lower());
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} for 'lower'.", error.what()));
      }
      try {
        Decoder<T>::decode(node["upper"], value.upper());
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} for 'upper'.", error.what()));
      }
    } else if (node.IsSequence() && node.size() == 2) {
      try {
        Decoder<T>::decode(node[0], value.lower());
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} at index {}.", error.what(), 0));
      }
      try {
        Decoder<T>::decode(node[1], value.upper());
      } catch (const YamlError &error) {
        throw YamlError(fmt::format("{} at index {}.", error.what(), 1));
      }
    } else {
      throw YamlError(fmt::format("Bad conversion to type '{}'", getTypeName(value)));
    }
  }
};

template <typename T, int N>
struct Decoder<Eigen::Matrix<T, N, 1>> {
  static void decode(const YAML::Node &node, Eigen::Matrix<T, N, 1> &value) {
    std::array<T, N> result;
    Decoder<std::array<T, N>>::decode(node, result);
    value = Eigen::Map<Eigen::Matrix<T, N, 1>>(result.data());
  }
};

template <typename T>
struct Decoder<Eigen::Matrix<T, -1, 1>> {
  static void decode(const YAML::Node &node, Eigen::Matrix<T, -1, 1> &value) {
    std::vector<T> result;
    if (value.size() != 0) result.resize(value.size());
    Decoder<std::vector<T>>::decode(node, result);
    value = Eigen::Map<Eigen::Matrix<T, -1, 1>>(result.data(), result.size());
  }
};

template <typename T, int N>
struct Decoder<Eigen::Array<T, N, 1>> {
  static void decode(const YAML::Node &node, Eigen::Array<T, N, 1> &value) {
    std::array<T, N> result;
    Decoder<std::array<T, N>>::decode(node, result);
    value = Eigen::Map<Eigen::Array<T, N, 1>>(result.data());
  }
};

template <typename T>
struct Decoder<Eigen::Array<T, -1, 1>> {
  static void decode(const YAML::Node &node, Eigen::Array<T, -1, 1> &value) {
    std::vector<T> result;
    if (value.size() != 0) result.resize(value.size());
    Decoder<std::vector<T>>::decode(node, result);
    value = Eigen::Map<Eigen::Array<T, -1, 1>>(result.data(), result.size());
  }
};

#if __cplusplus >= 201703L
template <typename T>
struct Decoder<std::optional<T>> {
  static void decode(const YAML::Node &node, std::optional<T> &value) {
    if (node.IsNull()) {
      value.reset();
      return;
    }
    T inner_value{};
    Decoder<T>::decode(node, inner_value);
    value = inner_value;
  }
};
#endif

struct Indices : std::vector<std::size_t> {
  Indices() = default;
  const std::vector<std::size_t> &canonicalize(std::size_t sequence_len) {
    clear();
    if (not raw_indices.empty()) {
      for (auto &index : raw_indices) {
        if (index < 0) index += static_cast<std::int64_t>(sequence_len);
        if (index < 0 or index >= static_cast<std::int64_t>(sequence_len)) {
          throw YamlError(fmt::format("Index {} is out of range for sequence of length {}", index, sequence_len));
        }
        push_back(static_cast<std::size_t>(index));
      }
    } else {
      std::int64_t start = slice.first, end = slice.second;
      if (start < 0) start += static_cast<std::int64_t>(sequence_len);
      if (end <= 0) end += static_cast<std::int64_t>(sequence_len);
      if (start < 0 or start >= end or end > static_cast<std::int64_t>(sequence_len)) {
        throw YamlError(fmt::format("Slice range [start={}, end={}) is invalid for sequence of length {}", slice.first,
                                    slice.second, sequence_len));
      }
      for (std::int64_t i{start}; i < end; ++i) push_back(static_cast<std::size_t>(i));
    }
    return *this;
  }

  std::vector<std::int64_t> raw_indices;
  std::pair<std::int64_t, std::int64_t> slice{0, 0};
};

template <>
struct Decoder<Indices> {
  static void decode(const YAML::Node &node, Indices &value) {
    if (not((node.IsSequence() and node.size() > 0) or node.IsMap())) {
      throw YamlError("Expected a non-empty sequence or a map for indices specification.");
    }

    if (node.IsSequence()) {
      return Decoder<std::vector<std::int64_t>>::decode(node, value.raw_indices);
    }
    YAML::Node indices_node = node["indices"];
    if (indices_node.IsDefined() and not indices_node.IsNull()) {
      if (not(indices_node.IsSequence() and indices_node.size() > 0)) {
        throw YamlError("Expected 'indices' to be a non-empty sequence.");
      }
      Decoder<std::vector<std::int64_t>>::decode(indices_node, value.raw_indices);
    } else {
      YAML::Node start_node = node["start"];
      YAML::Node end_node   = node["end"];
      if (start_node.IsDefined() and not start_node.IsNull()) {
        Decoder<std::int64_t>::decode(start_node, value.slice.first);
      }
      if (end_node.IsDefined() and not end_node.IsNull()) {
        Decoder<std::int64_t>::decode(end_node, value.slice.second);
      }
    }
  }
};
}  // namespace yml
}  // namespace llu

#endif  // LLU_YAML_H_
