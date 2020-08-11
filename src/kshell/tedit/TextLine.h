#pragma once

#include <stdint.h>
#include <stddef.h>

class TextLine final {
        TextLine(const TextLine &other)=delete;
        TextLine& operator=(const TextLine &other)=delete;
        TextLine(char * data,size_t len,size_t alloc);
    public:
        TextLine();
        TextLine(TextLine && other);
        TextLine& operator=(TextLine && other);
        
        ~TextLine();
        
        TextLine * split(size_t at);
        void append(const TextLine &other);
        
        void insert(char c,size_t at);
        void erase(size_t at);
        
        const char * get();
        
        size_t len();
        
        void trim();//reduce allocated size to used size
    private:
        void resize(size_t new_size,bool move_terminator);
        void grow(size_t to);
        char * buf;
        size_t _len;//size excluding null terminator
        size_t _alloc;//allocated size
};
