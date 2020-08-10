#ifndef FILE_INTERNAL_H_INCLUDED
#define FILE_INTERNAL_H_INCLUDED

#ifndef __cplusplus
#error file must be used only from c++
#endif // __cplusplus

#include "fs/vfs.h"
#include <stdio.h>

constexpr size_t UNGETC_BUFFER_SIZE=8;

enum file_flags {
    FLAG_READ=0x1,
    FLAG_WRITE=0x2,
    FLAG_BINARY=0x4,
    FLAG_EOF=0x8,
    FLAG_ERR=0x10,
    FLAG_BUFFERED_LINE=0x20,
    FLAG_BUFFERED_FULL=0x40,
    FLAG_UNBUFFERED=0x80,
    FLAG_APPEND=0x100,
};

struct k_FILE_HANDLE {
    FS::FileHandle * f_handle;
    size_t ungetc_location;
    size_t index;
    uint32_t flags;
    char * buffer;
    size_t buffer_size;
    size_t buffer_location;
    
    char ungetc_buffer[UNGETC_BUFFER_SIZE];
    
    //TODO
};

#endif // FILE_INTERNAL_H_INCLUDED
