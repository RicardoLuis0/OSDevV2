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

namespace Util{
    
    template<typename T,auto ACQUIRE,auto RELEASE>
    class generic_guard final {
        generic_guard(const generic_guard&other)=delete;
        generic_guard(generic_guard&&other)=delete;
        generic_guard&operator=(const generic_guard&other)=delete;
        generic_guard&operator=(generic_guard&&other)=delete;
        T & data;
    public:
        generic_guard(T & d):data(d){
            if constexpr(TMP::is_member_function_pointer<decltype(ACQUIRE)>::value){
                (data.*ACQUIRE)();
            }else{
                ACQUIRE(data);
            }
        }
        ~generic_guard(){
            if constexpr(TMP::is_member_function_pointer<decltype(RELEASE)>::value){
                (data.*RELEASE)();
            }else{
                RELEASE(data);
            }
        }
    };
    
    template<auto ACQUIRE,auto RELEASE>
    class simple_generic_guard final {
        simple_generic_guard(const simple_generic_guard&other)=delete;
        simple_generic_guard(simple_generic_guard&&other)=delete;
        simple_generic_guard&operator=(const simple_generic_guard&other)=delete;
        simple_generic_guard&operator=(simple_generic_guard&&other)=delete;
    public:
        simple_generic_guard(){
            ACQUIRE();
        }
        ~simple_generic_guard(){
            RELEASE();
        }
    };
    
}

#endif // UTIL_H_INCLUDED
