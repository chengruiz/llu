#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <llu/eigen.h>
#include <llu/yaml.h>

namespace {
llu::yml::Node inlineNode(std::string_view source) { return llu::yml::Node{YAML::Load(std::string{source})}; }

std::string writeTempYaml(std::string_view source) {
  static int counter = 0;
  const auto path = std::filesystem::temp_directory_path() / ("llu_yaml_test_" + std::to_string(counter++) + ".yaml");
  std::ofstream out(path);
  out << source;
  out.close();
  return path.string();
}

std::string captureWhat(const std::function<void()> &callback) {
  try {
    callback();
  } catch (const std::exception &error) {
    return error.what();
  }
  return {};
}
}  // namespace

TEST(LLU_YAML_TEST, NodePredicatesReflectYamlTypes) {
  const auto root      = inlineNode("map: {nested: 1}\nseq: [1, 2, 3]\nscalar: 2.5\nflag: true\nnone: null\n");
  const auto undefined = llu::yml::Node{};

  EXPECT_TRUE(root.isMap());
  EXPECT_TRUE(root.hasValue());
  EXPECT_TRUE(root.isDefined());

  EXPECT_TRUE(root["map"].isMap());
  EXPECT_TRUE(root["seq"].isSequence());
  EXPECT_TRUE(root["seq"].isSequence(3));
  EXPECT_TRUE(root["scalar"].isScalar());
  EXPECT_TRUE(root["scalar"].isFloat());
  EXPECT_TRUE(root["flag"].isBool());
  EXPECT_TRUE(root["scalar"].isType<double>());
  EXPECT_FALSE(root["scalar"].isType<int>());

  EXPECT_TRUE(root["none"].isDefined());
  EXPECT_TRUE(root["none"].isNull());
  EXPECT_FALSE(root["none"].hasValue());

  EXPECT_FALSE(undefined.isDefined());
  EXPECT_FALSE(undefined.hasValue());
  EXPECT_FALSE(undefined.isMap());
  EXPECT_FALSE(undefined.isScalar());
}

TEST(LLU_YAML_TEST, NodeAccessPreservesContextForNestedLookups) {
  auto root = inlineNode("outer:\n  inner:\n    - {value: 10}\n");

  const auto outer = root["outer"];
  const auto inner = outer["inner"];
  const auto item  = inner[0];
  const auto value = item["value"];

  ASSERT_EQ(outer.context().keys.size(), 1u);
  ASSERT_EQ(inner.context().keys.size(), 2u);
  ASSERT_EQ(item.context().keys.size(), 3u);
  ASSERT_EQ(value.context().keys.size(), 4u);

  EXPECT_TRUE(outer.context().top_node.IsMap());
  EXPECT_TRUE(value.isScalar());
  EXPECT_EQ(value.as<int>(), 10);
}

TEST(LLU_YAML_TEST, SequenceIteratorsWrapYamlNodesAsLluNodes) {
  const auto root = inlineNode("items:\n  - 3\n  - 5\n  - 8\n");

  std::vector<int> values;
  for (const auto item : root["items"]) {
    values.push_back(item.as<int>());
  }

  EXPECT_EQ(values, (std::vector<int>{3, 5, 8}));

  auto it = root["items"].cbegin();
  ASSERT_NE(it, root["items"].cend());
  EXPECT_EQ(it->as<int>(), 3);

  const auto message = captureWhat([&] { it->as<bool>(); });
  EXPECT_NE(message.find("[0]"), std::string::npos);
}

TEST(LLU_YAML_TEST, MapIteratorsExposeWrappedKeysAndValues) {
  const auto root = inlineNode("items:\n  alpha: 2\n  beta: 4\n");

  std::vector<std::string> keys;
  std::vector<int> values;
  for (const auto entry : root["items"]) {
    keys.push_back(entry.first.as<std::string>());
    values.push_back(entry.second.as<int>());
  }

  EXPECT_EQ(keys, (std::vector<std::string>{"alpha", "beta"}));
  EXPECT_EQ(values, (std::vector<int>{2, 4}));

  auto it = root["items"].begin();
  ASSERT_NE(it, root["items"].end());
  EXPECT_EQ(it->first.as<std::string>(), "alpha");
  EXPECT_EQ(it->second.as<int>(), 2);

  const auto message = captureWhat([&] { it->second.as<bool>(); });
  EXPECT_NE(message.find("[alpha]"), std::string::npos);
}

TEST(LLU_YAML_TEST, AssertionHelpersAcceptMatchingNodes) {
  auto root = inlineNode("flag: false\nnumber: 3.25\nitems: [1, 2, 3]\nname: hello\nmap: {x: 1}\n");

  EXPECT_NO_THROW(root.assertMap());
  EXPECT_NO_THROW(root.assertIterable());
  EXPECT_NO_THROW(root["flag"].assertBool());
  EXPECT_NO_THROW(root["number"].assertFloat());
  EXPECT_NO_THROW(root["items"].assertSequence());
  EXPECT_NO_THROW(root["items"].assertSequence(3));
  EXPECT_NO_THROW(root["items"].assertIterable());
  EXPECT_NO_THROW(root["name"].assertScalar());
  EXPECT_NO_THROW(root["map"].assertMap());
  EXPECT_NO_THROW(root["flag"].assertHasValue());
}

TEST(LLU_YAML_TEST, AssertionHelpersReportReadableErrors) {
  auto root = inlineNode("outer:\n  value: text\n");

  EXPECT_THROW(root["outer"]["missing"].assertDefined(), llu::yml::YamlError);
  EXPECT_THROW(root["outer"]["value"].assertBool(), llu::yml::YamlError);
  EXPECT_THROW(root["outer"].assertSequence(), llu::yml::YamlError);

  const auto message = captureWhat([&] { root["outer"]["value"].assertBool(); });
  EXPECT_NE(message.find("YamlError"), std::string::npos);
  EXPECT_NE(message.find("Expected a boolean value"), std::string::npos);
  EXPECT_NE(message.find("Full path:"), std::string::npos);
}

TEST(LLU_YAML_TEST, KeySetAssertionsHandleFalseyValuesAndConflicts) {
  auto root =
      inlineNode("map:\n  flag: false\n  count: 0\n  name: ''\n  primary: 1\n  secondary: 2\nscalar: 3\n");

  EXPECT_NO_THROW(root["map"].assertHasValue("flag", "count", "name"));
  EXPECT_NO_THROW(root["map"].assertMutuallyExclusive({"flag", "missing"}));

  const auto iterable_message = captureWhat([&] { root["scalar"].assertIterable(); });
  EXPECT_NE(iterable_message.find("Expected a sequence or map"), std::string::npos);
  EXPECT_NE(iterable_message.find("[scalar]"), std::string::npos);

  const auto missing_value_message = captureWhat([&] { root["map"].assertHasValue("flag", "missing"); });
  EXPECT_NE(missing_value_message.find("Expected a value for key 'missing'"), std::string::npos);
  EXPECT_NE(missing_value_message.find("[map]"), std::string::npos);

  const auto conflict_message = captureWhat([&] { root["map"].assertMutuallyExclusive({"primary", "secondary"}); });
  EXPECT_NE(conflict_message.find("Mutually exclusive keys"), std::string::npos);
  EXPECT_NE(conflict_message.find("[map]"), std::string::npos);
}

TEST(LLU_YAML_TEST, ScalarConversionsAndDefaultsWork) {
  auto root = inlineNode("integer: 7\nreal: 2.5\nflag: true\nnone: null\n");

  int integer_value = 0;
  double real_value = 0.;
  bool flag_value   = false;

  root["integer"].to(integer_value);
  root["real"].to(real_value);
  root["flag"].to(flag_value);

  EXPECT_EQ(integer_value, 7);
  EXPECT_NEAR(real_value, 2.5, llu::kEPS);
  EXPECT_TRUE(flag_value);

  EXPECT_EQ(root["integer"].as<int>(), 7);
  EXPECT_NEAR(root["real"].as<double>(), 2.5, llu::kEPS);
  EXPECT_EQ(root["missing"].as<int>(42), 42);
  EXPECT_EQ(root["none"].as<int>(42), 42);

  int preserved = 99;
  EXPECT_NO_THROW(root["missing"].to(preserved, true));
  EXPECT_EQ(preserved, 99);
  EXPECT_NO_THROW(root["none"].to(preserved, true));
  EXPECT_EQ(preserved, 99);
}

TEST(LLU_YAML_TEST, VectorAndArrayDecodersHandleScalarAndSequenceInputs) {
  auto root = inlineNode("scalar: 5\nints: [1, 2, 3]\nbools: [true, false, on, off]\ndoubles: [1.5, 2.5, 3.5]\n");

  std::vector<int> repeated(4);
  std::vector<int> ints;
  std::vector<bool> bools;
  std::array<double, 3> doubles{};

  root["scalar"].to(repeated);
  root["ints"].to(ints);
  root["bools"].to(bools);
  root["doubles"].to(doubles);

  EXPECT_EQ(repeated, (std::vector<int>{5, 5, 5, 5}));
  EXPECT_EQ(ints, (std::vector<int>{1, 2, 3}));
  ASSERT_EQ(bools.size(), 4u);
  EXPECT_TRUE(bools[0]);
  EXPECT_FALSE(bools[1]);
  EXPECT_TRUE(bools[2]);
  EXPECT_FALSE(bools[3]);
  EXPECT_NEAR(doubles[0], 1.5, llu::kEPS);
  EXPECT_NEAR(doubles[1], 2.5, llu::kEPS);
  EXPECT_NEAR(doubles[2], 3.5, llu::kEPS);

  std::array<int, 2> wrong_size{};
  std::vector<int> fixed_size(2);
  EXPECT_THROW(root["ints"].to(wrong_size), llu::yml::YamlError);
  EXPECT_THROW(root["ints"].to(fixed_size), llu::yml::YamlError);
}

TEST(LLU_YAML_TEST, VectorBoolDecoderValidatesPreSizedSequences) {
  auto root = inlineNode("flags: [true, false, true]\n");

  std::vector<bool> flags(2);

  const auto message = captureWhat([&] { root["flags"].to(flags); });

  EXPECT_NE(message.find("Expected size 2, but got 3"), std::string::npos);
  EXPECT_NE(message.find("[flags]"), std::string::npos);
}

TEST(LLU_YAML_TEST, ScalarInputsBroadcastAcrossContainersAndFixedEigenTypes) {
  auto root = inlineNode("scalar: 6\nflag: true\n");

  std::vector<int> single_value;
  std::vector<bool> repeated_flags(3);
  std::array<int, 3> repeated_array{};
  llu::Vec3d repeated_vec{};

  root["scalar"].to(single_value);
  root["flag"].to(repeated_flags);
  root["scalar"].to(repeated_array);
  root["scalar"].to(repeated_vec);

  EXPECT_EQ(single_value, (std::vector<int>{6}));
  ASSERT_EQ(repeated_flags.size(), 3u);
  EXPECT_TRUE(repeated_flags[0]);
  EXPECT_TRUE(repeated_flags[1]);
  EXPECT_TRUE(repeated_flags[2]);
  EXPECT_EQ(repeated_array, (std::array<int, 3>{6, 6, 6}));
  EXPECT_TRUE(repeated_vec.isApprox(llu::Vec3d::Constant(6.)));
}

TEST(LLU_YAML_TEST, RangeDecoderSupportsScalarMapAndSequenceForms) {
  auto root = inlineNode("scalar: 4.5\nmap: {lower: 1.5, upper: 6.5}\nseq: [2.5, 3.5]\nbad: [1.0, 2.0, 3.0]\n");

  llu::range_t<double> scalar_range;
  llu::range_t<double> map_range;
  llu::range_t<double> sequence_range;

  root["scalar"].to(scalar_range);
  root["map"].to(map_range);
  root["seq"].to(sequence_range);

  EXPECT_NEAR(scalar_range.lower(), 4.5, llu::kEPS);
  EXPECT_NEAR(scalar_range.upper(), 4.5, llu::kEPS);
  EXPECT_NEAR(map_range.lower(), 1.5, llu::kEPS);
  EXPECT_NEAR(map_range.upper(), 6.5, llu::kEPS);
  EXPECT_NEAR(sequence_range.lower(), 2.5, llu::kEPS);
  EXPECT_NEAR(sequence_range.upper(), 3.5, llu::kEPS);

  EXPECT_THROW(root["bad"].to(scalar_range), llu::yml::YamlError);
}

TEST(LLU_YAML_TEST, EigenDecodersSupportFixedAndDynamicShapes) {
  auto root = inlineNode("triplet: [1, 2, 3]\nscalar: 8\n");

  llu::Vec3d fixed_vec{};
  llu::Arr3d fixed_arr{};
  llu::VecXd dynamic_vec;
  llu::ArrXd dynamic_arr;

  root["triplet"].to(fixed_vec);
  root["triplet"].to(fixed_arr);
  root["triplet"].to(dynamic_vec);
  root["triplet"].to(dynamic_arr);

  const llu::Vec3d expected_triplet{1., 2., 3.};
  EXPECT_TRUE(fixed_vec.isApprox(expected_triplet));
  EXPECT_TRUE(fixed_arr.isApprox(expected_triplet.array()));
  EXPECT_TRUE(dynamic_vec.isApprox(expected_triplet));
  EXPECT_TRUE(dynamic_arr.isApprox(expected_triplet.array()));

  dynamic_vec.resize(3);
  dynamic_arr.resize(3);
  root["scalar"].to(dynamic_vec);
  root["scalar"].to(dynamic_arr);

  const llu::Vec3d expected_scalar{8., 8., 8.};
  EXPECT_TRUE(dynamic_vec.isApprox(expected_scalar));
  EXPECT_TRUE(dynamic_arr.isApprox(expected_scalar.array()));

  llu::VecXd wrong_size(2);
  EXPECT_THROW(root["triplet"].to(wrong_size), llu::yml::YamlError);
}

#if __cplusplus >= 201703L
TEST(LLU_YAML_TEST, OptionalDecoderHandlesValuesAndNulls) {
  auto root = inlineNode("present: 11\nmissing: null\n");

  std::optional<int> present;
  std::optional<int> missing = 7;

  root["present"].to(present);
  root["missing"].to(missing);

  ASSERT_TRUE(present.has_value());
  EXPECT_EQ(*present, 11);
  EXPECT_FALSE(missing.has_value());
}

TEST(LLU_YAML_TEST, OptionalDecoderReportsUndefinedNodes) {
  auto root = inlineNode("present: 11\n");

  std::optional<int> missing = 7;

  const auto message = captureWhat([&] { root["missing"].to(missing); });

  EXPECT_NE(message.find("Undefined node"), std::string::npos);
  EXPECT_NE(message.find("Full path:"), std::string::npos);
  EXPECT_NE(message.find("[missing]"), std::string::npos);
}

TEST(LLU_YAML_TEST, OptionalDecoderPropagatesInnerConversionErrors) {
  auto root = inlineNode("bad: nope\n");

  std::optional<int> value;

  const auto message = captureWhat([&] { root["bad"].to(value); });

  EXPECT_NE(message.find("Failed to decode as int"), std::string::npos);
  EXPECT_NE(message.find("Full path:"), std::string::npos);
  EXPECT_NE(message.find("[bad]"), std::string::npos);
}
#endif

TEST(LLU_YAML_TEST, GetDefinedKeyReturnsFirstPresentValue) {
  auto root = inlineNode("primary: null\nsecondary: 3\ntertiary: 4\n");

  EXPECT_EQ(root.getDefinedKey({"missing", "primary", "secondary", "tertiary"}), "secondary");
  EXPECT_EQ(root.getDefinedKey({"missing", "absent"}), "");
  EXPECT_THROW(root["secondary"].getDefinedKey({"x", "y"}), llu::yml::YamlError);
}

TEST(LLU_YAML_TEST, LoadFileReadsYamlAndStoresFilenameContext) {
  const auto path = writeTempYaml("root:\n  child: 7\n");

  const auto loaded = llu::yml::loadFile(path);

  EXPECT_TRUE(loaded.isMap());
  EXPECT_EQ(loaded.context().filename, path);
  EXPECT_TRUE(loaded.context().top_node.IsMap());
  EXPECT_EQ(loaded["root"]["child"].as<int>(), 7);
}

TEST(LLU_YAML_TEST, LoadFileReturnsEmptyNodeWhenMissingFileIsAllowed) {
  const auto path = (std::filesystem::temp_directory_path() / "llu_yaml_test_missing.yaml").string();
  std::filesystem::remove(path);

  const auto loaded = llu::yml::loadFile(path, true);

  EXPECT_FALSE(loaded.hasValue());
  EXPECT_EQ(loaded.context().filename, path);
}

TEST(LLU_YAML_TEST, LoadFileThrowsBadFileErrorForMissingFiles) {
  const auto path = (std::filesystem::temp_directory_path() / "llu_yaml_test_missing_required.yaml").string();
  std::filesystem::remove(path);

  EXPECT_THROW(llu::yml::loadFile(path), llu::yml::BadFileError);

  const auto message = captureWhat([&] { llu::yml::loadFile(path); });
  EXPECT_NE(message.find("Failed to load YAML file"), std::string::npos);
  EXPECT_NE(message.find(path), std::string::npos);
}

TEST(LLU_YAML_TEST, ConversionErrorsIncludeFileAndPathContext) {
  const auto path = writeTempYaml("outer:\n  inner:\n    value: not-an-int\n");
  auto loaded     = llu::yml::loadFile(path);

  const auto message = captureWhat([&] {
    int value = 0;
    loaded["outer"]["inner"]["value"].to(value);
  });

  EXPECT_NE(message.find("YamlError"), std::string::npos);
  EXPECT_NE(message.find("Failed to decode as int"), std::string::npos);
  EXPECT_NE(message.find("File:"), std::string::npos);
  EXPECT_NE(message.find(path), std::string::npos);
  EXPECT_NE(message.find("Full path:"), std::string::npos);
}

TEST(LLU_YAML_TEST, ScalarAccessAndMissingChainsProduceReadableErrors) {
  auto root = inlineNode("scalar: 1\nouter: {}\n");

  const auto scalar_access_message = captureWhat([&] { static_cast<void>(root["scalar"]["child"]); });
  EXPECT_NE(scalar_access_message.find("Cannot access key 'child' on a scalar node"), std::string::npos);
  EXPECT_NE(scalar_access_message.find("Full path:"), std::string::npos);
  EXPECT_NE(scalar_access_message.find("[scalar]"), std::string::npos);

  const auto missing_leaf = root["outer"]["missing"]["leaf"];
  EXPECT_FALSE(missing_leaf.hasValue());

  const auto missing_chain_message = captureWhat([&] { missing_leaf.assertHasValue(); });
  EXPECT_NE(missing_chain_message.find("Expected a value"), std::string::npos);
  EXPECT_NE(missing_chain_message.find("Full path:"), std::string::npos);
  EXPECT_NE(missing_chain_message.find("[outer][missing][leaf]"), std::string::npos);
}

TEST(LLU_YAML_TEST, MissingAndNullConversionsRemainStrictWithoutDefaults) {
  auto root = inlineNode("none: null\n");

  const auto missing_message = captureWhat([&] {
    int value = 0;
    root["missing"].to(value);
  });
  EXPECT_NE(missing_message.find("Undefined node"), std::string::npos);
  EXPECT_NE(missing_message.find("[missing]"), std::string::npos);

  const auto null_message = captureWhat([&] {
    int value = 0;
    root["none"].to(value);
  });
  EXPECT_NE(null_message.find("Failed to decode as int"), std::string::npos);
  EXPECT_NE(null_message.find("[none]"), std::string::npos);
}

TEST(LLU_YAML_TEST, ContainerAndRangeErrorsReportPreciseLocations) {
  auto root = inlineNode(
      "bad_vector: [1, nope, 3]\n"
      "bad_array: [1.0, nope, 3.0]\n"
      "bad_range_lower: {lower: nope, upper: 4.0}\n"
      "bad_range_upper: {lower: 1.0, upper: nope}\n"
      "bad_range_seq: [1.0, nope]\n");

  const auto vector_message = captureWhat([&] {
    std::vector<int> values;
    root["bad_vector"].to(values);
  });
  EXPECT_NE(vector_message.find("at index 1"), std::string::npos);
  EXPECT_NE(vector_message.find("[bad_vector]"), std::string::npos);

  const auto array_message = captureWhat([&] {
    std::array<double, 3> values{};
    root["bad_array"].to(values);
  });
  EXPECT_NE(array_message.find("at index 1"), std::string::npos);
  EXPECT_NE(array_message.find("[bad_array]"), std::string::npos);

  const auto lower_message = captureWhat([&] {
    llu::range_t<double> value;
    root["bad_range_lower"].to(value);
  });
  EXPECT_NE(lower_message.find("for 'lower'"), std::string::npos);
  EXPECT_NE(lower_message.find("[bad_range_lower]"), std::string::npos);

  const auto upper_message = captureWhat([&] {
    llu::range_t<double> value;
    root["bad_range_upper"].to(value);
  });
  EXPECT_NE(upper_message.find("for 'upper'"), std::string::npos);
  EXPECT_NE(upper_message.find("[bad_range_upper]"), std::string::npos);

  const auto sequence_message = captureWhat([&] {
    llu::range_t<double> value;
    root["bad_range_seq"].to(value);
  });
  EXPECT_NE(sequence_message.find("at index 1"), std::string::npos);
  EXPECT_NE(sequence_message.find("[bad_range_seq]"), std::string::npos);
}

TEST(LLU_YAML_TEST, GetDefinedKeyTreatsFalseyScalarsAsPresentValues) {
  auto root = inlineNode("flag: false\ncount: 0\nname: ''\n");

  EXPECT_EQ(root.getDefinedKey({"missing", "flag", "count"}), "flag");
  EXPECT_EQ(root.getDefinedKey({"missing", "count", "name"}), "count");
  EXPECT_EQ(root.getDefinedKey({"missing", "name"}), "name");
}

TEST(LLU_YAML_TEST, LoadFilePropagatesYamlSyntaxErrors) {
  const auto path = writeTempYaml("root: [1, 2\n");

  EXPECT_THROW(llu::yml::loadFile(path), YAML::Exception);
  EXPECT_THROW(llu::yml::loadFile(path, true), YAML::Exception);
}
