#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"

#include "util/tmp.h"

template<typename T> constexpr T min(T a,T b){return a<b?a:b;}
template<typename T> constexpr T max(T a,T b){return a>b?a:b;}
template<typename T> constexpr T clamp(T a,T b,T c){return c<a?a:c>b?b:c;}

constexpr unsigned long long int operator "" _KB(unsigned long long int i){
    return i*1024;
}

constexpr unsigned long long int operator "" _MB(unsigned long long int i){
    return i*1024*1024;
}

constexpr unsigned long long int operator "" _GB(unsigned long long int i){
    return i*1024*1024*1024;
}

constexpr uint32_t align_size(size_t size,size_t align){
    if(size%align){
        return size;
    }else{
        return align*((size/align)+1);
    }
}

#endif // UTIL_H_INCLUDED
