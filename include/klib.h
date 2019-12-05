#ifndef KLIB_H_INCLUDED
#define KLIB_H_INCLUDED

#include "default/stdint.h"

//C-compatible header for internal kernel library

#ifdef __cplusplus
constexpr void * NULL=0;
extern "C" {
#else
#define NULL ((void*)0)
#endif // __cplusplus

void * k_malloc(uint32_t size);
void k_abort();
void k_abort_i(int code);
void k_abort_s(const char * msg);

#ifdef __cplusplus
}
#endif

#endif // KLIB_H_INCLUDED
