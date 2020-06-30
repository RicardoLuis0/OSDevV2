#ifndef KLIB_H_INCLUDED
#define KLIB_H_INCLUDED

#include <stdint.h>
#include <stddef.h>
#include <string.h>

//C-compatible header for internal kernel library

#ifdef __cplusplus
#include "new.h"
#include "util.h"
extern "C" {
#define NORETURN [[noreturn]]
#else
#define NORETURN __attribute__((noreturn))
#endif // __cplusplus

void * malloc(size_t size);
//void * k_malloc_x(size_t size,size_t align);
void * calloc(size_t num,size_t size);
void * realloc(void * ptr,size_t size);
void free(void * ptr);

NORETURN void k_halt();
NORETURN void k_abort();
NORETURN void k_abort_fullscreen();
NORETURN void k_abort_i(int code);
NORETURN void k_abort_s(const char * msg);
NORETURN void k_abort_s_i_s(const char * s1,int i,const char * s2);
NORETURN void k_abort_assert(const char * condition,const char * name,uint32_t line);
NORETURN void k_abort_massert(const char * condition,const char * msg,const char * name,uint32_t line);

//screen
void k_putc(char c);
void k_puts(const char * s);
void k_putsn(const char * s,size_t n);
void k_cls();
size_t k_get_y();
size_t k_get_x();
void k_set_xy(size_t x,size_t y);

void k_puti(int32_t);//print int
void k_putu(int32_t);//print unsigned int
void k_putll(int64_t);//print long long
void k_putull(int64_t);//print unsigned long long
void k_puth(uint64_t);//print unsigned long long as hex
void k_putmem(uint64_t);//print unsigned long long as memory (ex. 10M 2K 10B)

int k_getch();

enum k_getch_ext_keys{
    K_GETCH_EXT_UP=256,
    K_GETCH_EXT_DOWN,
    K_GETCH_EXT_LEFT,
    K_GETCH_EXT_RIGHT,
    K_GETCH_EXT_CTRL,
};
int k_getch_extended();

size_t k_hash_s(const char * s);
int k_strcmp_bool(const char * s1,const char * s2);//returns true/false whether strings are equal, not same as c stdlib strcmp

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

#ifdef __cplusplus
}
#endif

#undef NORETURN

#endif // KLIB_H_INCLUDED
