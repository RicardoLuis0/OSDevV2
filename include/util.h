#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "default/stdint.h"

template<typename T> constexpr T min(T a,T b){return a<b?a:b;}
template<typename T> constexpr T max(T a,T b){return a>b?a:b;}
template<typename T> constexpr T clamp(T a,T b,T c){return c<a?a:c>b?b:c;}

inline void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
#endif // UTIL_H_INCLUDED
