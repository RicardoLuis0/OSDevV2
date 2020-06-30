#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED


#include "stdc/stddef.h"
#include "stdc/stdarg.h"

#ifdef __cplusplus
extern "C" {
#define RESTRICT __restrict__
#else
#define RESTRICT restrict
#endif // __cplusplus

#define EOF -1
#define stdin ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)

#define BUFSIZ 512

struct k_FILE {
    size_t temp;//
    //TODO
};

typedef struct k_FILE FILE;

void putc(char c,FILE*);
void puts(const char * s);
FILE * fopen(const char * filename,const char * mode);
FILE * freopen(const char * filename,const char * mode,FILE *);
FILE * tmpfile();
int getc(FILE *);
int ferror(FILE *);
int fclose(FILE *);
char * fgets(char * str, int n, FILE *);


int printf(const char * RESTRICT fmt, ...);
int vprintf(const char * RESTRICT fmt, va_list arg);
int sprintf(char * RESTRICT buf,const char * RESTRICT fmt, ...);
int snprintf(char * RESTRICT buf,size_t n,const char * RESTRICT fmt, ...);
int vsprintf( char * RESTRICT buf, const char * RESTRICT fmt, va_list arg);
int vsnprintf( char * RESTRICT buf, size_t n, const char * RESTRICT fmt, va_list arg);


int feof(FILE*);
int fread(void*,size_t,size_t,FILE*);

//TODO


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STDIO_H_INCLUDED
