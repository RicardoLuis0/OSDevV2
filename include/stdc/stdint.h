#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED

#define join(a,b) a##b
#define join_indirect(a,b) join(a,b)

#include <stddef.h>

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#define INT32_C(i) join_indirect(i, __INT32_C_SUFFIX__)
#define UINT32_C(i) join_indirect(i##U, __INT32_C_SUFFIX__)

#endif // STDINT_H_INCLUDED
