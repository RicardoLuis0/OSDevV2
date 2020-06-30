#include "klib.h"
#include <stdio.h>
#include <errno.h>

extern "C" {
    int errno;
}
const char * err_unknown = "unknown error";
char * errstr=(char*)err_unknown;

extern "C" char * strerror(int errnum) {
    return errstr;
}

extern "C" [[noreturn]] void abort() {
    k_abort();
}

//source http://www.cse.yorku.ca/~oz/hash.html
size_t k_hash_s(const char * s){
    size_t hash = 5381;
    while(*s++)hash = ((hash << 5) + hash) + ((size_t)*s); /* hash * 33 + c */
    return hash;
}

int k_strcmp_bool(const char * s1,const char * s2){
    while(*s1||*s2){
        if(*s1++!=*s2++)return false;
    }
    return true;
}
