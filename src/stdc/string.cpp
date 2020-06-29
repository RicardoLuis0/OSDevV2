#include "klib.h"
#include <stdio.h>
#include <errno.h>
#include <limits.h>

extern "C" {
    int tolower(int c) {
        return (c>='a'&&c<='z')?c-('a'-'A'):c;
    }
    int toupper(int c) {
        return (c>='A'&&c<='Z')?c-('A'-'a'):c;
    }
    int isspace (int c){
        return c==' '||c=='\n'||c=='\t'||c=='\r'||c=='\v'||c=='\f';
    }
    char * strrchr(const char * s,int c){
        size_t i = 0;
        while (s[i++]);
        do{
            if(s[--i]==(char)c) {
                return(char *)s+i;
            }
        } while(i);
        return NULL;
    }
    extern const char * _digits;
    extern const char * _Xdigits;
    
    static uintmax_t _strtox_main( const char ** p, unsigned int base, uintmax_t error, uintmax_t limval, int limdigit, char * sign ) {
        uintmax_t rc = 0;
        int digit = -1;
        const char * x;

        while ( ( x = (const char *)memchr( _digits, tolower( **p ), base ) ) != NULL )
        {
            digit = x - _digits;

            if ( ( rc < limval ) || ( ( rc == limval ) && ( digit <= limdigit ) ) )
            {
                rc = rc * base + ( unsigned )digit;
                ++( *p );
            }
            else
            {
                errno = ERANGE;

                /* TODO: Only if endptr != NULL - but do we really want *another* parameter? */
                /* TODO: Earlier version was missing tolower() here but was not caught by tests */
                while ( memchr( _digits, tolower( **p ), base ) != NULL )
                {
                    ++( *p );
                }

                /* TODO: This is ugly, but keeps caller from negating the error value */
                *sign = '+';
                return error;
            }
        }

        if ( digit == -1 )
        {
            *p = NULL;
            return 0;
        }

        return rc;
    }
    
    
    static const char * _strtox_prelim( const char * p, char * sign, int * base ) {
        /* skipping leading whitespace */
        while ( isspace( *p ) ) {
            ++p;
        }

        /* determining / skipping sign */
        if ( *p != '+' && *p != '-' ) {
            *sign = '+';
        } else {
            *sign = *( p++ );
        }

        /* determining base */
        if ( *p == '0' ) {
            ++p;

            if ( ( *base == 0 || *base == 16 ) && ( *p == 'x' || *p == 'X' ) ) {
                *base = 16;
                ++p;

                /* catching a border case here: "0x" followed by a non-digit should
                   be parsed as the unprefixed zero.
                   We have to "rewind" the parsing; having the base set to 16 if it
                   was zero previously does not hurt, as the result is zero anyway.
                */
                if ( memchr( _digits, tolower( *p ), *base ) == NULL ) {
                    p -= 2;
                }
            } else if ( *base == 0 ) {
                *base = 8;
                /* back up one digit, so that a plain zero is decoded correctly
                   (and endptr is set correctly as well).
                   (2019-01-15, Giovanni Mascellani)
                */
                --p;
            } else {
                --p;
            }
        } else if ( ! *base ) {
            *base = 10;
        }

        return ( ( *base >= 2 ) && ( *base <= 36 ) ) ? p : NULL;
    }
    
    long int strtol( const char * s, char ** endptr, int base ) {
        long int rc;
        char sign = '+';
        const char * p = _strtox_prelim( s, &sign, &base );
        if ( base < 2 || base > 36 ) {
            return 0;
        }
        if ( sign == '+' ) {
            rc = ( long int )_strtox_main( &p, ( unsigned )base, ( uintmax_t )LONG_MAX, ( uintmax_t )( LONG_MAX / base ), ( int )( LONG_MAX % base ), &sign );
        } else {
            rc = ( long int )_strtox_main( &p, ( unsigned )base, ( uintmax_t )LONG_MIN, ( uintmax_t )( LONG_MIN / -base ), ( int )( -( LONG_MIN % base ) ), &sign );
        }
        if ( endptr != NULL ) {
            *endptr = ( p != NULL ) ? ( char * ) p : ( char * ) s;
        }
        return ( sign == '+' ) ? rc : -rc;
    }

    void * memchr( const void * s, int c, size_t n ) {
        const unsigned char * p = ( const unsigned char * ) s;
        while ( n-- ) {
            if ( *p == ( unsigned char ) c ) {
                return ( void * ) p;
            }
            ++p;
        }
        return NULL;
    }

    void * memcpy(void *dst, const void *src, size_t n) {
        uint8_t *d=(uint8_t*)dst; 
        const uint8_t *s=(const uint8_t*)src;
        for(;n>0;n--,d++,s++){
            *d=*s;
        }
        return dst;
    }

    void * memset(void *src, int data, size_t n) {
        unsigned char * p=(unsigned char *)src;
        while(n-->0)*(p++)=(unsigned char)data;
        return src;
    }

    char * strdup(const char * s){
        size_t l=strlen(s)+1;
        char * d=(char*)calloc(l,sizeof(char));
        for(size_t i=0;i<l;i++){
            d[i]=s[i];
        }
        return d;
    }

    char * strndup(const char * s,size_t n){
        char * d=(char*)calloc(n+1,sizeof(char));
        for(size_t i=0;i<n;i++){
            d[i]=s[i];
        }
        d[n]='\0';
        return d;
    }

    size_t strlen(const char * s){
        size_t count=0;
        while(*s++)count++;
        return count;
    }

    //memmove from musl (https://github.com/esmil/musl/blob/master/src/string/memmove.c)

    /*
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
    constexpr uint32_t sz=sizeof(size_t);
    void * memmove(void * dst,const void * src,size_t n) {
        char *d = (char*)dst;
        const char *s = (const char *)src;

        if (d==s) return d;
        if (s+n <= d || d+n <= s) return memcpy(d, s, n);

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
}


