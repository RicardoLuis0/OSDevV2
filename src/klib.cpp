#include "klib.h"
#include <stdio.h>
#include <errno.h>
#include <util/hash_table.h>
#include <util/smart_ptr.h>

extern "C" {
    int errno;
}

Util::HashTable<Util::UniquePtr<char>> * kernel_env=nullptr;

void k_env_init(){
    kernel_env=new Util::HashTable<Util::UniquePtr<char>>();
}

extern "C" char * getenv(const char* name){
    return kernel_env->at_else(name,NULL);
}

extern "C" int setenv(const char *name,const char *value,int overwrite){
    if(overwrite||!kernel_env->has(name)){
        (*kernel_env)[name]=strdup(value);
    }
    return 0;
}

extern "C" int unsetenv(const char *name){
    kernel_env->unset(name);
    return 0;
}

extern "C" int __cxa_atexit(void (*func) (void *), void * arg, void * d){
    k_abort_s("__cxa_atexit unimplemented");
}
const char * err_unknown = "unknown error";
char * errstr=(char*)err_unknown;

extern "C" char * strerror(int errnum) {
    return errstr;
}

extern "C" [[noreturn]] void abort() {
    k_abort();
}

extern "C" int system(const char *command){
    if(command==NULL){
        return 0;
    }else{
        return -1;
    }
}

void klib_init(){
    k_env_init();
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
