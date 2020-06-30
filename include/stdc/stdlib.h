#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED

#include "stdc/stddef.h"

#ifdef __cplusplus
extern "C" {
#define NORETURN [[noreturn]]
#else
#define NORETURN __attribute__((noreturn))
#endif // __cplusplus

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void * malloc(size_t size);
void * calloc(size_t num,size_t size);
void * realloc(void * ptr,size_t size);
void free(void * ptr);
int abs(int);
long int strtol (const char* str, char** endptr, int base);
double strtod(const char * s, char ** p);

char * getenv(const char* name);
int setenv(const char *name,const char *value,int overwrite);
int unsetenv(const char *name);
int system(const char *command);


NORETURN void abort();
NORETURN void exit(int);
int atexit(void(*func)(void));

//TODO

#ifdef __cplusplus
}
#endif // __cplusplus

#undef NORETURN

#endif // STDLIB_H_INCLUDED
