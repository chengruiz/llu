#ifndef LLU_MACRO_H_
#define LLU_MACRO_H_

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define LLU_TO_STR_IMPL(...) #__VA_ARGS__
#define LLU_TO_STR(text) LLU_TO_STR_IMPL(text)

#define LLU_FILELINE() "[" __FILE_NAME__ ":" LLU_TO_STR(__LINE__) "]"

#endif  // LLU_MACRO_H_
