#include "klib.h"

constexpr uint32_t sz=sizeof(size_t);

/*
memmove from musl (https://github.com/esmil/musl/blob/master/src/string/memmove.c)

musl as a whole is licensed under the following standard MIT license:
----------------------------------------------------------------------
Copyright © 2005-2014 Rich Felker, et al.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
*/

extern "C" void * k_memmove(void * dst,const void * src,size_t n) {
    char *d = (char*)dst;
    const char *s = (const char *)src;

    if (d==s) return d;
    if (s+n <= d || d+n <= s) return k_memcpy(d, s, n);

    if (d<s) {
        if ((uintptr_t)s % sz == (uintptr_t)d % sz) {
            while ((uintptr_t)d % sz) {
                if (!n--) return dst;
                *d++ = *s++;
            }
            for (; n>=sz; n-=sz, d+=sz, s+=sz) *(size_t *)d = *(size_t *)s;
        }
        for (; n; n--) *d++ = *s++;
    } else {
        if ((uintptr_t)s % sz == (uintptr_t)d % sz) {
            while ((uintptr_t)(d+n) % sz) {
                if (!n--) return dst;
                d[n] = s[n];
            }
            while (n>=sz) n-=sz, *(size_t *)(d+n) = *(size_t *)(s+n);
        }
        while (n) n--, d[n] = s[n];
    }

    return dst;
}

extern "C" void * k_memcpy(void *dst, const void *src, size_t n) {//copy
    uint8_t *d=(uint8_t*)dst; 
    const uint8_t *s=(const uint8_t*)src;
    for(;n>0;n--,d++,s++){
        *d=*s;
    }
    return dst;
}

extern "C" void * k_memset(void *src, int data, size_t n) {
    unsigned char * p=(unsigned char *)src;
    while(n-->0)*(p++)=(unsigned char)data;
    return src;
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

char * k_strdup(const char * s){
    size_t l=k_strlen(s)+1;
    char * d=(char*)k_calloc(l,sizeof(char));
    for(size_t i=0;i<l;i++){
        d[i]=s[i];
    }
    return d;
}

char * k_strndup(const char * s,size_t n){
    char * d=(char*)k_calloc(n+1,sizeof(char));
    for(size_t i=0;i<n;i++){
        d[i]=s[i];
    }
    d[n]='\0';
    return d;
}

size_t k_strlen(const char * s){
    size_t count=0;
    while(*s++)count++;
    return count;
}
