#ifndef LLU_MACRO_H_
#define LLU_MACRO_H_

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LLU_TO_STR_IMPL(...) #__VA_ARGS__
#define LLU_TO_STR(text) LLU_TO_STR_IMPL(text)

#define LLU_FILELINE() "[" __FILE_NAME__ ":" LLU_TO_STR(__LINE__) "]"

#if __cplusplus >= 201703L
#include <type_traits>
#define LLU_ASSERT_IS_FLOATING_POINT(T) static_assert(std::is_floating_point_v<T>)
#else
#define LLU_ASSERT_IS_FLOATING_POINT(T)
#endif

#endif  // LLU_MACRO_H_
