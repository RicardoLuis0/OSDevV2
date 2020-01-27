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

void * k_malloc(size_t size);
void * k_calloc(size_t num,size_t size);
void * k_realloc(void * ptr,size_t size);
void k_free(void * ptr);
NORETURN void k_abort();
NORETURN void k_abort_i(int code);
NORETURN void k_abort_s(const char * msg);
NORETURN void k_abort_s_i_s(const char * s1,int i,const char * s2);
NORETURN void k_abort_assert(const char * condition,const char * name,uint32_t line);
NORETURN void k_abort_massert(const char * condition,const char * msg,const char * name,uint32_t line);
void k_putc(char c);
void k_puts(const char * s);
char k_getch();
void * k_memmove(void * dst,const void * src,size_t n);
void * k_memcpy(void *dst,const void *src,size_t n);
void * k_memset(void *dst,int data,size_t n);
size_t k_hash_s(const char * s);
int k_strcmp_bool(const char * s1,const char * s2);//returns true/false whether strings are equal, not same as c stdlib strcmp
char * k_strdup(const char * s);
char * k_strndup(const char * s,size_t n);
size_t k_strlen(const char * s);
void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);

#ifdef __cplusplus
}
#endif

#undef NORETURN

#endif // KLIB_H_INCLUDED
