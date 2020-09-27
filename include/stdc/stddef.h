#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED


#ifdef __cplusplus
typedef decltype(nullptr) nullptr_t;
#define NULL nullptr
#else
#define NULL ((void*)0)
#endif // __cplusplus
#define offsetof(st, m) ((size_t)&(((st *)0)->m))
#include <stdint.h>
typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ptrdiff_t;
#define SIZE_MAX __SIZE_MAX__

#endif // STDDEF_H_INCLUDED
