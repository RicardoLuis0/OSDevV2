#include "klib.h"
#include <stdio.h>
#include <stdlib.h>
#include <file_internal.h>
#include <errno.h>
extern "C" {
    
    int feof(FILE * f){
        if(f==stdin||f==stdout||f==stderr){
            return false;
        }else{
            return f->flags&FLAG_EOF;
        }
    }
    
    int fread(void * buf,size_t sz,size_t n,FILE * f){
        if(f==stdin){
            k_abort_s("STDIN fread unimplemented");
        }else if(f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            k_abort_s("Trying to read from write mode file");
        }else{
            if(f->ungetc_location==0){
                size_t fsz=f->f_handle->size();
                if(f->flags&FLAG_EOF){
                    return 0;
                }else if(f->index>fsz){
                    f->flags|=FLAG_EOF;
                    return 0;
                }
                size_t c=f->f_handle->read(buf,sz,n,f->index);
                f->index+=c;
                if(f->index>fsz){
                    f->flags|=FLAG_EOF;
                }
                return c;
            }else{
                k_abort_s("ungetc support unimplemented");
            }
        }
    }
    
    int fgetc(FILE * f){
        if(f==stdin){
            return k_getch();
        }else if(f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            k_abort_s("Trying to read from write mode file");
        }else{
            if(f->ungetc_location==0){
                return f->flags&FLAG_EOF?EOF:({char c2=f->f_handle->getc(f->index);f->index++;c2;});
            }else{
                k_abort_s("ungetc support unimplemented");
            }
        }
    }
    
    int getc(FILE * f){
        return fgetc(f);
    }
    
    char * fgets(char * str, int n, FILE * f){
        if(f==stdin){
            return k_gets(str,n);
        }else if(f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            k_abort_s("Trying to read from write mode file");
        }else{
            if(f->ungetc_location==0){
                size_t fsz=f->f_handle->size();
                for(int i=0;i<n;i++){
                    str[i]=f->f_handle->getc(f->index);
                    f->index++;
                    if(f->index>fsz){
                        f->flags|=FLAG_EOF;
                        if(i==0)return nullptr;
                        break;
                    }
                }
                return str;
            }else{
                k_abort_s("ungetc support unimplemented");
            }
        }
    }
    
    int ungetc(int c, FILE * f){
        if(f==stdin){
            return k_getch();
        }else if(f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            k_abort_s("Trying to ungetc into read mode file");
        }else{
            if(f->index==0) return 1;
            if(f->ungetc_location==0&&c==f->f_handle->getc(f->index-1)){//if ungetc buffer is empty and char is same as file char, just decrement index
                f->index--;
            }else{
                k_abort_s("fungetc unimplemented");
            }
            return 0;
        }
    }
    
    void clearerr(FILE * f){
        if(f!=stdin&&f!=stdout&&f!=stderr){
            f->flags&=~(FLAG_EOF|FLAG_ERR);
        }
    }
    
    int fputs(const char * str,FILE * f){
        if(f==stdout||f==stderr){
            k_puts(str);
            return 0;
        }else if(f==stdin||!(f->flags&(FLAG_WRITE|FLAG_APPEND))){
            k_abort_s("Trying to write into read mode file");
        }else{
            size_t n=strlen(str);
            return fwrite(str,1,n,f);
        }
    }
    
    int fputc(char c,FILE * f){
        if(f==stdout||f==stderr){
            k_putc(c);
            return 0;
        }else if(f==stdin||!(f->flags&(FLAG_WRITE|FLAG_APPEND))){
            k_abort_s("Trying to write into read mode file");
        }else{
            return fwrite(&c,1,1,f);
        }
    }
    
    size_t fwrite(const void * buf,size_t sz,size_t n,FILE * f){
        if(f==stdout||f==stderr){
            k_abort_s("STDOUT,STDERR fwrite unimplemented");
        }else if(f==stdin||!(f->flags&(FLAG_WRITE|FLAG_APPEND))){
            k_abort_s("Trying to write into read mode file");
        }else if(f->flags&FLAG_APPEND){
            if(f->flags&FLAG_BUFFERED_FULL||f->flags&FLAG_BUFFERED_LINE){
                k_abort_s("buffered file output unimplemented");
            }
            size_t c=f->f_handle->write(buf,sz,n,f->f_handle->size());
            f->index=f->f_handle->size();
            return c;
        }else{
            if(f->flags&FLAG_BUFFERED_FULL||f->flags&FLAG_BUFFERED_LINE){
                k_abort_s("buffered file output unimplemented");
            }
            if(f->flags&FLAG_EOF)f->index=f->f_handle->size();
            size_t c=f->f_handle->write(buf,sz,n,f->index);
            f->index+=c;
            return c;
        }
    }
    
    int fseek(FILE * f,long offset,int origin){
        if(f==stdin){
            return 0;
        }else if(f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            k_abort_s("Trying to fseek write mode file");
        }else{
            size_t fsz=f->f_handle->size();
            switch(origin){
            case SEEK_SET:
                if(offset<0){
                    f->index=0;
                    if(f->flags&FLAG_EOF){
                        f->flags&=~FLAG_EOF;
                    }
                }else{
                    f->index=offset;
                    if(f->index>=fsz){
                        f->flags|=FLAG_EOF;
                    }else if(f->flags&FLAG_EOF){
                        f->flags&=~FLAG_EOF;
                    }
                }
                return 0;
            case SEEK_CUR:
                if(offset<0&&(abs(offset)>f->index)){
                    f->index=0;
                    f->ungetc_location=0;
                    if(f->flags&FLAG_EOF){
                        f->flags&=~FLAG_EOF;
                    }
                }else{
                    f->index+=offset;
                    f->ungetc_location=0;
                    if(f->index>=fsz){
                        f->flags|=FLAG_EOF;
                    }else if(f->flags&FLAG_EOF){
                        f->flags&=~FLAG_EOF;
                    }
                }
                return 0;
            case SEEK_END:
                f->index=fsz;
                f->ungetc_location=0;
                f->flags|=FLAG_EOF;
                return 0;
            default:
                return 1;
            }
        }
    }
    
    long ftell(FILE * f){
        if(f==stdin||f==stdout||f==stderr||!(f->flags&FLAG_READ)){
            return 0;
        }else{
            return f->flags&(FLAG_EOF|FLAG_APPEND)?f->f_handle->size():f->index-f->ungetc_location;
        }
    }
    
    int setvbuf(FILE * f,char *buf,int mode,size_t size){
        k_abort_s("setvbuf unimplemented");
    }
    
    int remove(const char * filename){
        return FS::VFSRoot::removeFile(nullptr,filename);
    }
    
    int rename(const char * old_filename, const char * new_filename){
        return FS::VFSRoot::renameFile(nullptr,old_filename,new_filename);
    }
    
    int fflush(FILE * f){
        if(f==stdout||f==stderr||f==stdin){
            return 0;
        }else{
            if(f->flags&FLAG_BUFFERED_FULL||f->flags&FLAG_BUFFERED_LINE){
                k_abort_s("buffered file unimplemented");
            }
        }
        return 0;
    }
    
    enum {
        MODE_READ=0x1,
        MODE_WRITE=0x2,
        MODE_APPEND=0x4,
        MODE_RW=0x8,
        MODE_BINARY=0x10,
        MODE_NO_OVERWRITE=0x20,
    };
    
    static size_t parse_mode(const char * mode_str){
        size_t mode=0;
        switch(mode_str[0]){//read base mode
        case  'r':
            mode|=MODE_READ;
            break;
        case 'w':
            mode|=MODE_WRITE;
            break;
        case 'a':
            mode|=MODE_APPEND;
            break;
        case '\0':
            return 0;//prevent buffer overruns for empty mode strings
        }
        for(size_t i=1;mode_str[i]!='\0';i++) switch(mode_str[i]){//read modifiers
        case '+'://read/write mode
            mode|=MODE_RW;
            break;
        case 'b'://binary mode, do nothing
            mode|=MODE_BINARY;
            break;
        case 'x'://don't overwrite file if exists
            if(mode&MODE_WRITE)mode|=MODE_NO_OVERWRITE;
            break;
        }
        return mode;
    }
    
    FILE * fopen(const char * filename,const char * mode_str){
        size_t mode=parse_mode(mode_str);
        
        if(!(mode&MODE_READ||mode&MODE_WRITE||mode&MODE_APPEND))return nullptr;//invalid mode
        FS::FileHandle * fh;
        if(mode&MODE_NO_OVERWRITE){
            fh=FS::VFSRoot::resolveFilePathCreate(nullptr,filename);
        }else{
            fh=FS::VFSRoot::resolveFilePath(nullptr,filename,(mode&(MODE_WRITE|MODE_APPEND)));
        }
        
        if(!fh)return nullptr;//could not open/create file
        fh->lock();
        k_FILE_HANDLE * f=new k_FILE_HANDLE;
        f->f_handle=fh;
        f->flags=FLAG_UNBUFFERED;
        f->buffer=nullptr;
        f->buffer_size=0;
        f->buffer_location=0;
        f->ungetc_location=0;
        if(mode&MODE_READ){
            f->flags|=FLAG_READ;
            f->index=0;
            if(mode&MODE_RW){
                f->flags|=FLAG_WRITE;
            }
        }else if(mode&MODE_WRITE){
            f->flags|=FLAG_WRITE;
            f->index=0;
            fh->clear();
            if(mode&MODE_RW){
                f->flags|=FLAG_READ;
            }
        }else if(mode&MODE_APPEND){
            f->flags|=FLAG_APPEND;
            f->index=fh->size();
            if(mode&MODE_RW){
                f->flags|=FLAG_READ;
            }
        }
        return f;
    }
    
    FILE * freopen(const char * filename,const char * mode,FILE * f){
        k_abort_s("freopen unimplemented");
    }
    
    int fclose(FILE * f){
        if(f){
            fflush(f);
            f->f_handle->unlock();
            delete f;
        }
        return 0;
    }
    
    FILE * tmpfile(){
        return NULL;
    }
    
    char * tmpnam(char * s){
        k_abort_s("ungetc unimplemented");
    }
    
    int ferror(FILE *){
        return 0;
    }
    
}
