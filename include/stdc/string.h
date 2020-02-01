#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void * k_memmove(void * dst,const void * src,size_t n);
void * k_memcpy(void *dst,const void *src,size_t n);
void * k_memset(void *dst,int data,size_t n);
char * k_strdup(const char * s);
char * k_strndup(const char * s,size_t n);
char * k_strcpy(char * d,const char * s);
char * k_strcat(char * d,const char * s);
size_t k_strlen(const char * s);
char * k_strchr(char * str,int c);
char * k_strrchr(char * str,int c);
char * k_strstr(char * s1,const char * s2);
int k_strcmp(const char * s1,const char * s2);
int k_strncmp(const char * s1,const char * s2,size_t n);
int k_memcmp(const void * p1,const void * p2,size_t n);//TODO implement

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STRING_H_INCLUDED
