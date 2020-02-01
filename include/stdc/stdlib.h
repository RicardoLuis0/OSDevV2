#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void * k_malloc(size_t size);
void * k_calloc(size_t num,size_t size);
void * k_realloc(void * ptr,size_t size);
void k_free(void * ptr);

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STDLIB_H_INCLUDED
