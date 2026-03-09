#include <gtest/gtest.h>

#include <llu/typename.h>

struct TypeA {};
class TypeB {};

TEST(LLU_TYPENAME_TEST, TemplateOverloadReturnsDemangledBuiltinNames) {
  EXPECT_EQ(llu::getTypeName<int>(), "int");
  EXPECT_EQ(llu::getTypeName<float>(), "float");
  EXPECT_EQ(llu::getTypeName<double>(), "double");
}

TEST(LLU_TYPENAME_TEST, TemplateOverloadReturnsDemangledUserTypeNames) {
  EXPECT_EQ(llu::getTypeName<TypeA>(), "TypeA");
  EXPECT_EQ(llu::getTypeName<TypeB>(), "TypeB");
}

TEST(LLU_TYPENAME_TEST, ValueOverloadReturnsDemangledRuntimeTypeNames) {
  EXPECT_EQ(llu::getTypeName(TypeA{}), "TypeA");
  EXPECT_EQ(llu::getTypeName(TypeB{}), "TypeB");
}
