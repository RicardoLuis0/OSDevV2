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
#define NORETURN __attribute__((__noreturn__))
#endif // __cplusplus

#define FOR2(s) s s
#define FOR4(s) FOR2(s) FOR2(s)
#define FOR8(s) FOR4(s) FOR4(s)
#define FOR16(s) FOR8(s) FOR8(s)
#define FOR32(s) FOR16(s) FOR16(s)
#define FOR64(s) FOR32(s) FOR32(s)
#define FOR128(s) FOR64(s) FOR64(s)
#define FOR256(s) FOR128(s) FOR128(s)
#define FOR512(s) FOR256(s) FOR256(s)

#define FORC2(s) s , s
#define FORC4(s) FORC2(s) , FORC2(s)
#define FORC8(s) FORC4(s) , FORC4(s)
#define FORC16(s) FORC8(s) , FORC8(s)
#define FORC32(s) FORC16(s) , FORC16(s)
#define FORC64(s) FORC32(s) , FORC32(s)
#define FORC128(s) FORC64(s) , FORC64(s)
#define FORC256(s) FORC128(s) , FORC128(s)
#define FORC512(s) FORC256(s) , FORC256(s)

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
void k_logs(const char * c);//write to both VGA and Serial
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

void k_sleep(uint64_t);

int k_getch();
char * k_gets(char * str, int n);

enum k_getch_ext_keys{
    K_GETCH_EXT_UP=256,
    K_GETCH_EXT_DOWN,
    K_GETCH_EXT_LEFT,
    K_GETCH_EXT_RIGHT,
    K_GETCH_EXT_CTRL,
    K_GETCH_EXT_ALT,
};
int k_getch_extended();

size_t k_hash_s(const char * s);
int k_strcmp_bool(const char * s1,const char * s2);//returns true/false whether strings are equal, not same as c stdlib strcmp

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t val);
uint16_t inw(uint16_t port);

void outl(uint16_t port, uint32_t val);
uint32_t inl(uint16_t port);

#ifdef __cplusplus

}

namespace extra {
    template<typename T> struct always_false : TMP::conditional<TMP::is_same<T,void>::value,TMP::true_type,TMP::false_type>::type {};//a bit ugly but it works
    template<> struct always_false<void> : TMP::false_type {};
}

template<typename T> void out(uint16_t port,T val){
    static_assert(extra::always_false<T>::value,"invalid type, must be uint of size 8/16/32");
}

template<typename T> T in(uint16_t port){
    static_assert(extra::always_false<T>::value,"invalid type, must be uint of size 8/16/32");
}

template<> inline void out<uint8_t>(uint16_t port,uint8_t val){
    outb(port,val);
}

template<> inline uint8_t in<uint8_t>(uint16_t port){
    return inb(port);
}

template<> inline void out<uint16_t>(uint16_t port,uint16_t val){
    outw(port,val);
}

template<> inline uint16_t in<uint16_t>(uint16_t port){
    return inw(port);
}

template<> inline void out<uint32_t>(uint16_t port,uint32_t val){
    outl(port,val);
}

template<> inline uint32_t in<uint32_t>(uint16_t port){
    return inl(port);
}

#endif

#undef NORETURN

#endif // KLIB_H_INCLUDED
