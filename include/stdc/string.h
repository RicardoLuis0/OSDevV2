#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void * memmove(void * dst,const void * src,size_t n);
void * memcpy(void *dst,const void *src,size_t n);
void * memset(void *dst,int data,size_t n);
void * memchr(const void *s, int c, size_t n);
char * strdup(const char * s);
char * strndup(const char * s,size_t n);
char * strcpy(char * d,const char * s);
char * strcat(char * d,const char * s);
size_t strlen(const char * s);
char * strchr(const char * str,int c);
char * strrchr(const char * str,int c);
char * strstr(const char * s1,const char * s2);
size_t strspn(const char * s1, const char * s2 );
int strcmp(const char * s1,const char * s2);
int strncmp(const char * s1,const char * s2,size_t n);
int memcmp(const void * p1,const void * p2,size_t n);//TODO implement


int strcoll(const char * str1,const char * str2);

char * strpbrk(const char * s1, const char * s2);

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STRING_H_INCLUDED
