#ifndef LLU_MACRO_H_
#define LLU_MACRO_H_

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LLU_TO_STR_IMPL(...) #__VA_ARGS__
#define LLU_TO_STR(text)     LLU_TO_STR_IMPL(text)

#define LLU_FILELINE() "[" __FILE_NAME__ ":" LLU_TO_STR(__LINE__) "]"

#include <type_traits>
#if __cplusplus >= 201703L
#define LLU_ASSERT_FP(T)  static_assert(std::is_floating_point_v<T>)
#define LLU_ASSERT_INT(T) static_assert(std::is_integral<T>())
#else
#define LLU_ASSERT_FP(T)  static_assert(true, "LLU_ASSERT_FP failed.")
#define LLU_ASSERT_INT(T) static_assert(std::is_integral<T>(), "LLU_ASSERT_INT failed.")
#endif

#endif  // LLU_MACRO_H_
