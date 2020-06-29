#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void * malloc(size_t size);
void * calloc(size_t num,size_t size);
void * realloc(void * ptr,size_t size);
void free(void * ptr);
int abs(int);
long int strtol (const char* str, char** endptr, int base);

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STDLIB_H_INCLUDED
