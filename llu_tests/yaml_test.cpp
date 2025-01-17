#include <gtest/gtest.h>

#include <llu/eigen.h>
#include <llu/yaml.h>


TEST(LLU_YAML_TEST, LLU_TYPE_TEST) {
  YAML::Node node = YAML::Load("{a: 1, b: [1, 2, 3], c: 2.5, d: false}");
  ASSERT_TRUE(llu::yml::isType<int>(node["a"])) << "Expected a int";
  ASSERT_TRUE(llu::yml::isNTuple(node["b"], 3)) << "Expected a 3-tuple";
  ASSERT_TRUE(llu::yml::isType<double>(node["c"])) << "Expected a double";
  ASSERT_TRUE(llu::yml::isFloat(node["c"])) << "Expected a int";
  ASSERT_TRUE(llu::yml::isType<bool>(node["d"])) << "Expected a bool";
  ASSERT_TRUE(llu::yml::isBool(node["d"])) << "Expected a int";
  ASSERT_FALSE(llu::yml::isNTuple(node["d"], 2)) << "Expected not a 2-tuple";
  ASSERT_FALSE(llu::yml::isType<int>(node["d"])) << "Expected not a int";
  llu::yml::assertNTuple(node["b"], 3);
  llu::yml::assertNTuple(node, "b", 3);
}

TEST(LLU_YAML_TEST, LLU_VALID_TEST) {
  YAML::Node node = YAML::Load("{a: 1, b: [1, 2, 3], c: 2.5, d: false}");
  ASSERT_TRUE(llu::yml::isValid(node)) << "Expected a valid node";
  ASSERT_TRUE(llu::yml::isValid(node, "a")) << "Expected a valid node";
  ASSERT_TRUE(llu::yml::isValid(node, "b", 1)) << "Expected a valid node";
  ASSERT_FALSE(llu::yml::isValid(node, "b", 3)) << "Expected an invalid node";
  ASSERT_FALSE(llu::yml::isValid(node, "not_a_key")) << "Expected an invalid node";
  ASSERT_FALSE(llu::yml::isValid(node, "not_a_key", "not_a_key")) << "Expected an invalid node";

  llu::yml::assertValid(node, "a");
  llu::yml::assertValid(node, "b");
  llu::yml::assertValid(node, "c");
  llu::yml::assertValid(node, "d");
  try {
    llu::yml::assertValid(node, "not_a_key");
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}

  try {
    llu::yml::assertNTuple(node, "a", 2);
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}
}

TEST(LLU_YAML_TEST, LLU_READ_TEST) {
  YAML::Node node = YAML::Load("{a: 1, b: [1, 2, 3], c: 2.5, d: true}");
  int a{};
  std::vector<int> b1{};
  std::vector<int> b1_expected{1, 2, 3};
  std::array<double, 3> b2{};
  double c{};
  bool d{};

  llu::yml::setTo(node["a"], a);
  llu::yml::setTo(node["b"], b1);
  llu::yml::setTo(node["b"], b2);
  llu::yml::setTo(node["c"], c);
  llu::yml::setTo(node["d"], d);
  ASSERT_EQ(a, 1) << "Expected a = 1";
  ASSERT_EQ(b1, b1_expected) << "Expected b = [1, 2, 3]";
  ASSERT_NEAR(b2[0], 1., llu::kEPS) << "Expected b[0] = 1";
  ASSERT_NEAR(b2[1], 2., llu::kEPS) << "Expected b[1] = 2";
  ASSERT_NEAR(b2[2], 3., llu::kEPS) << "Expected b[2] = 3";
  ASSERT_EQ(c, 2.5) << "Expected c = 2.5";
  ASSERT_TRUE(d) << "Expected d = true";

  a = {};
  b1 = {};
  b2 = {};
  c = {};
  d = {};
  llu::yml::setTo(node, "a", a);
  llu::yml::setTo(node, "b", b1);
  llu::yml::setTo(node, "b", b2);
  llu::yml::setTo(node, "c", c);
  llu::yml::setTo(node, "d", d);
  ASSERT_EQ(a, 1) << "Expected a = 1";
  ASSERT_EQ(b1, b1_expected) << "Expected b = [1, 2, 3]";
  ASSERT_NEAR(b2[0], 1., llu::kEPS) << "Expected b[0] = 1";
  ASSERT_NEAR(b2[1], 2., llu::kEPS) << "Expected b[1] = 2";
  ASSERT_NEAR(b2[2], 3., llu::kEPS) << "Expected b[2] = 3";
  ASSERT_EQ(c, 2.5) << "Expected c = 2.5";
  ASSERT_TRUE(d) << "Expected d = true";

  a = {};
  b1 = {};
  b2 = {};
  c = {};
  d = {};
  llu::yml::setIf(node["a"], a);
  llu::yml::setIf(node["b"], b1);
  llu::yml::setIf(node["b"], b2);
  llu::yml::setIf(node["c"], c);
  llu::yml::setIf(node["d"], d);
  ASSERT_EQ(a, 1) << "Expected a = 1";
  ASSERT_EQ(b1, b1_expected) << "Expected b = [1, 2, 3]";
  ASSERT_NEAR(b2[0], 1., llu::kEPS) << "Expected b[0] = 1";
  ASSERT_NEAR(b2[1], 2., llu::kEPS) << "Expected b[1] = 2";
  ASSERT_NEAR(b2[2], 3., llu::kEPS) << "Expected b[2] = 3";
  ASSERT_EQ(c, 2.5) << "Expected c = 2.5";
  ASSERT_TRUE(d) << "Expected d = true";

  a = {};
  b1 = {};
  b2 = {};
  c = {};
  d = {};
  llu::yml::setIf(node, "a", a);
  llu::yml::setIf(node, "b", b1);
  llu::yml::setIf(node, "b", b2);
  llu::yml::setIf(node, "c", c);
  llu::yml::setIf(node, "d", d);
  ASSERT_EQ(a, 1) << "Expected a = 1";
  ASSERT_EQ(b1, b1_expected) << "Expected b = [1, 2, 3]";
  ASSERT_NEAR(b2[0], 1., llu::kEPS) << "Expected b[0] = 1";
  ASSERT_NEAR(b2[1], 2., llu::kEPS) << "Expected b[1] = 2";
  ASSERT_NEAR(b2[2], 3., llu::kEPS) << "Expected b[2] = 3";
  ASSERT_EQ(c, 2.5) << "Expected c = 2.5";
  ASSERT_TRUE(d) << "Expected d = true";
}

TEST(LLU_YAML_TEST, LLU_READ_TEST2) {
  YAML::Node node = YAML::Load("{a: 1, b: [1, 2, 3], c: 2.5, d: true}");
  llu::Vec3d vec3{};
  llu::VecXd vecx{};
  llu::Arr3d arr3{};
  llu::ArrXd arrx{};
  llu::Vec3d expected{1., 2., 3.};

  llu::yml::setTo(node, "b", vec3);
  llu::yml::setTo(node, "b", vecx);
  llu::yml::setTo(node, "b", arr3);
  llu::yml::setTo(node, "b", arrx);

  ASSERT_TRUE(vec3.isApprox(expected)) << "Expected vec3 = [1, 2, 3]";
  ASSERT_TRUE(vecx.isApprox(expected)) << "Expected vecx = [1, 2, 3]";
  ASSERT_TRUE(arr3.isApprox(expected.array())) << "Expected arr3 = [1, 2, 3]";
  ASSERT_TRUE(arrx.isApprox(expected.array())) << "Expected arrx = [1, 2, 3]";

  llu::yml::setTo(node, "a", vec3);
  llu::yml::setTo(node, "a", arr3);
  expected = {1., 1., 1.};
  ASSERT_TRUE(vec3.isApprox(expected)) << "Expected vec3 = [1, 1, 1]";
  ASSERT_TRUE(arr3.isApprox(expected.array())) << "Expected arr3 = [1, 1, 1]";
}

TEST(LLU_YAML_TEST, LLU_READ_TEST3) {
  YAML::Node node = YAML::Load("{a: 1, b: null, c: [2.5, 3.5]}");
  std::optional<int> a;
  llu::yml::setTo(node, "a", a);
  ASSERT_TRUE(a.has_value()) << "Expected a.has_value()";
  ASSERT_EQ(a.value(), 1) << "Expected a = 1";
  llu::yml::setTo(node, "b", a);
  ASSERT_FALSE(a.has_value()) << "Expected not a.has_value()";

  llu::range_t<double> c;
  llu::yml::setTo(node, "c", c);
  ASSERT_NEAR(c.lower(), 2.5, llu::kEPS) << "Expected c.lower() = 2.5";
  ASSERT_NEAR(c.upper(), 3.5, llu::kEPS) << "Expected c.upper() = 3.5";

  ASSERT_EQ(llu::yml::readAs<int>(node, "a"), 1) << "Expected a = 1";
  ASSERT_EQ(llu::yml::readIf<int>(node, "a", 2), 1) << "Expected a = 1";
  ASSERT_EQ(llu::yml::readIf<int>(node, "d", 2), 2) << "ReadIf expected 2";
}

TEST(LLU_YAML_TEST, LLU_READ_TEST4) {
  YAML::Node node = YAML::Load("{a: 1, b: [1, 2, 3], c: 2.5, d: true}");
  int a{};
  std::array<int, 4> b1{};
  llu::Vec4d b2;

  try {
    llu::yml::setTo(node, "not_a_key", a);
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}

  a = -1;
  llu::yml::setIf(node, "not_a_key", a);
  ASSERT_EQ(a, -1) << "Expected a = -1";

  try {
    llu::yml::setTo(node, "b", b1);
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}

  try {
    llu::yml::setTo(node, "b", b2);
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}

  try {
    llu::yml::getItem(node, "e");
    ASSERT_TRUE(false) << "Expected an exception";
  } catch (const std::runtime_error &e) {}
}
