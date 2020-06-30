#include "klib.h"
#include <stdio.h>
#include <errno.h>
extern "C" {
    int feof(FILE*){
        k_abort_s("File operations not supported");
    }
    int fread(void*,size_t,size_t,FILE*){
        k_abort_s("File operations not supported");
    }
    int getc (FILE *) {
        k_abort_s("File operations not supported");
    }
    char * fgets(char * str, int n, FILE *){
        k_abort_s("File operations not supported");
    }
    int ungetc(int c, FILE *){
        k_abort_s("File operations not supported");
    }
    void clearerr(FILE *){
        
    }
    size_t fwrite(const void * ptr,size_t sz,size_t n,FILE *){
        k_abort_s("File operations not supported");
    }
    long ftell(FILE *){
        k_abort_s("File operations not supported");
    }
    int setvbuf(FILE *,char *buf,int type,size_t size){
        k_abort_s("File operations not supported");
    }
    int fflush(FILE *){
        return 0;
    }
    FILE * fopen(const char *,const char *){
        return NULL;
    }
    FILE * freopen(const char *,const char *,FILE *){
        return NULL;
    }
    FILE * tmpfile(){
        return NULL;
    }
    int ferror(FILE *){
        return 0;
    }
    int fclose(FILE *){
        return 0;
    }
}
