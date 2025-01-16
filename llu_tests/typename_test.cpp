#include <gtest/gtest.h>

#include <llu/typename.h>

struct TypeA {};
class TypeB {};

TEST(LLU_TYPENAME_TEST, LLU_TYPENAME_TEST) {
  ASSERT_EQ(llu::getTypeName<int>(), "int") << "Type name of int should be 'int'";
  ASSERT_EQ(llu::getTypeName<float>(), "float") << "Type name of float should be 'float'";
  ASSERT_EQ(llu::getTypeName<double>(), "double") << "Type name of double should be 'double'";
  ASSERT_EQ(llu::getTypeName<TypeA>(), "TypeA") << "Type name of TypeA should be 'TypeA'";
  ASSERT_EQ(llu::getTypeName<TypeB>(), "TypeB") << "Type name of TypeB should be 'TypeB'";
  ASSERT_EQ(llu::getTypeName(TypeA{}), "TypeA") << "Type name of TypeA{} should be 'TypeA'";
  ASSERT_EQ(llu::getTypeName(TypeB{}), "TypeB") << "Type name of TypeB{} should be 'TypeB'";
}
