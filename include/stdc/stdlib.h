#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#define NORETURN [[noreturn]]
#else
#define NORETURN __attribute__((noreturn))
#endif // __cplusplus

void * malloc(size_t size);
void * calloc(size_t num,size_t size);
void * realloc(void * ptr,size_t size);
void free(void * ptr);
int abs(int);
long int strtol (const char* str, char** endptr, int base);

NORETURN void abort();

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#undef NORETURN

#endif // STDLIB_H_INCLUDED
