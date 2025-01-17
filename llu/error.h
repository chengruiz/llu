#ifndef LLU_ERROR_H_
#define LLU_ERROR_H_

#include <fmt/core.h>

#include <llu/const.h>
#include <llu/macro.h>

#define LLU_ERROR(...)             throw std::runtime_error(fmt::format(LLU_FILELINE() " " __VA_ARGS__))
#define LLU_ASSERT_NOT(cond, ...)  if (cond) LLU_ERROR(__VA_ARGS__)
#define LLU_ASSERT(cond, ...)      LLU_ASSERT_NOT(not (cond), __VA_ARGS__)

#endif //  LLU_ERROR_H_
