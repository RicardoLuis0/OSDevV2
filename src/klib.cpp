#include "klib.h"

constexpr uint32_t sz=sizeof(uint32_t);

/*
from musl (https://github.com/esmil/musl/blob/master/src/string/memmove.c)

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

extern "C" void * memmove(void * dst,const void * src,uint32_t n) {
    char *d = (char*)dst;
    const char *s = (const char *)src;

    if (d==s) return d;
    //if (s+n <= d || d+n <= s) return memcpy(d, s, n);

    if (d<s) {
        if ((uintptr_t)s % sz == (uintptr_t)d % sz) {
            while ((uintptr_t)d % sz) {
                if (!n--) return dst;
                *d++ = *s++;
            }
            for (; n>=sz; n-=sz, d+=sz, s+=sz) *(uint32_t *)d = *(uint32_t *)s;
        }
        for (; n; n--) *d++ = *s++;
    } else {
        if ((uintptr_t)s % sz == (uintptr_t)d % sz) {
            while ((uintptr_t)(d+n) % sz) {
                if (!n--) return dst;
                d[n] = s[n];
            }
            while (n>=sz) n-=sz, *(uint32_t *)(d+n) = *(uint32_t *)(s+n);
        }
        while (n) n--, d[n] = s[n];
    }

    return dst;
}
