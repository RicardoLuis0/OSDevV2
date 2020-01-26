#ifndef KLIB_H_INCLUDED
#define KLIB_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"

//C-compatible header for internal kernel library

#ifdef __cplusplus
#include "new.h"
#include "util.h"
extern "C" {
#define NORETURN [[noreturn]]
#else
#define NORETURN
#endif // __cplusplus

void * k_malloc(uint32_t size);
void k_free(void * ptr);
NORETURN void k_abort();
NORETURN void k_abort_i(int code);
NORETURN void k_abort_s(const char * msg);
NORETURN void k_abort_assert(const char * msg,const char * filename,uint32_t line);
void k_putc(char c);
void k_puts(const char * s);
char k_getch();
void * k_memmove(void * dst,const void * src,size_t n);
void * k_memcpy(void *dst,const void *src,size_t n);
void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);

#ifdef __cplusplus
}
#endif

#undef NORETURN

#endif // KLIB_H_INCLUDED
