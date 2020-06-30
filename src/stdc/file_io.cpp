#include "klib.h"
#include <stdio.h>
#include <errno.h>
extern "C" {

    int feof(FILE * f){
        k_abort_s("File operations not supported");
    }

    int fread(void*,size_t,size_t,FILE * f){
        k_abort_s("File operations not supported");
    }

    int getc (FILE * f) {
        if(f==stdin){
            return k_getch();
        }else{
            k_abort_s("File operations not supported");
        }
    }

    char * fgets(char * str, int n, FILE * f){
        if(f==stdin){
            return k_gets(str,n);
        }else{
            k_abort_s("File operations not supported");
        }
    }

    int ungetc(int c, FILE * f){
        k_abort_s("File operations not supported");
    }

    void clearerr(FILE * f){
        
    }

    int fputs(const char * str,FILE * f){
        if(f==stdout||f==stderr){
            k_puts(str);
            return 0;
        }else{
            k_abort_s("File operations not supported");
        }
    }

    size_t fwrite(const void * ptr,size_t sz,size_t n,FILE * f){
        k_abort_s("File operations not supported");
    }

    int fseek(FILE * f,long offset,int origin){
        k_abort_s("File operations not supported");
    }

    long ftell(FILE * f){
        k_abort_s("File operations not supported");
    }

    int setvbuf(FILE * f,char *buf,int type,size_t size){
        k_abort_s("File operations not supported");
    }

    int remove(const char * filename){
        k_abort_s("File operations not supported");
    }

    int rename(const char * old_filename, const char * new_filename){
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

    char * tmpnam(char * s){
        k_abort_s("File operations not supported");
    }

    int ferror(FILE *){
        return 0;
    }

    int fclose(FILE *){
        return 0;
    }

}
