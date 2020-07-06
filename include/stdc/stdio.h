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
    void * f_handle;//FS::FolderHandle
    size_t index;
    size_t size;
    //TODO
};

typedef struct k_FILE FILE;

void putc(char c,FILE*);
void puts(const char * s);
FILE * fopen(const char * filename,const char * mode);
FILE * freopen(const char * filename,const char * mode,FILE *);
FILE * tmpfile();
void clearerr(FILE *);
int ferror(FILE *);
int fclose(FILE *);
enum fseek_origin {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};
int fseek(FILE *,long offset,int origin);
long ftell(FILE *);
enum vbuf_type{
    _IONBF,
    _IOFBF,
    _IOLBF,
};
int setvbuf(FILE *,char *buf,int type,size_t size);
int fflush(FILE *);
int getc(FILE *);
int ungetc(int c,FILE *);
char * fgets(char * str, int n,FILE *);
int fputs ( const char * str,FILE *);
size_t fwrite(const void * ptr,size_t sz,size_t n,FILE *);
int remove(const char *filename);
int rename(const char *old_filename, const char *new_filename);

int printf(const char * RESTRICT fmt, ...);
int vprintf(const char * RESTRICT fmt, va_list arg);
int sprintf(char * RESTRICT buf,const char * RESTRICT fmt, ...);
int snprintf(char * RESTRICT buf,size_t n,const char * RESTRICT fmt, ...);
int vsprintf( char * RESTRICT buf, const char * RESTRICT fmt, va_list arg);
int vsnprintf( char * RESTRICT buf, size_t n, const char * RESTRICT fmt, va_list arg);
int fprintf(FILE *, const char * RESTRICT fmt, ...);
int vfprintf(FILE *, const char * RESTRICT fmt, va_list arg);

#define L_tmpnam 30 //not specific

char * tmpnam(char * str);

int feof(FILE*);
int fread(void*,size_t,size_t,FILE*);

//TODO


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STDIO_H_INCLUDED
