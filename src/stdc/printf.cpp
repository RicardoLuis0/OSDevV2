#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "klib.h"

const char * _digits = "0123456789abcdefghijklmnopqrstuvwxyz";
const char * _Xdigits = "0123456789ABCDEF";

extern "C" {
    
    int printf(const char * RESTRICT fmt, ...){
        va_list arg;
        va_start(arg,fmt);
        int n=vprintf(fmt,arg);
        va_end(arg);
        return n;
    }
    
    int vprintf(const char * RESTRICT fmt, va_list arg){
        va_list a2;
        va_copy(a2,arg);
        int n=vsnprintf(NULL,0,fmt,a2);
        va_end(a2);
        char * s=(char*)calloc(n+1,sizeof(char));
        vsnprintf(s,n,fmt,arg);
        k_putsn(s,n);
        free(s);
        return n;
    }
    
    int sprintf(char * RESTRICT s,const char * RESTRICT fmt, ...){
        va_list arg;
        va_start(arg,fmt);
        int n=vsnprintf(s,SIZE_MAX,fmt,arg);
        va_end(arg);
        return n;
    }
    
    int snprintf(char * RESTRICT s,size_t n,const char * RESTRICT fmt, ...){
        va_list arg;
        va_start(arg,fmt);
        int n2=vsnprintf(s,n,fmt,arg);
        va_end(arg);
        return n2;
    }
    
    int vsprintf( char * RESTRICT buf, const char * RESTRICT fmt, va_list arg){
        return vsnprintf(buf,SIZE_MAX,fmt,arg);
    }

    int fprintf(FILE * f, const char * RESTRICT fmt, ...){
        va_list arg;
        va_start(arg,fmt);
        int n=vfprintf(f,fmt,arg);
        va_end(arg);
        return n;
    }

    int vfprintf(FILE * f, const char * RESTRICT fmt, va_list arg){
        va_list a2;
        va_copy(a2,arg);
        int n=vsnprintf(NULL,0,fmt,a2);
        va_end(a2);
        char * s=(char*)calloc(n+1,sizeof(char));
        vsnprintf(s,n,fmt,arg);
        fwrite(s,sizeof(char),n,f);
        free(s);
        return n;
    }
}
