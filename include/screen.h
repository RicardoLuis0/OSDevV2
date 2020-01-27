#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"

namespace Screen {
    enum color {
        BLACK = 0,
        BLUE = 1,
        GREEN = 2,
        CYAN = 3,
        RED = 4,
        MAGENTA = 5,
        BROWN = 6,
        LIGHT_GREY = 7,
        DARK_GREY = 8,
        LIGHT_BLUE = 9,
        LIGHT_GREEN = 10,
        LIGHT_CYAN = 11,
        LIGHT_RED = 12,
        LIGHT_MAGENTA = 13,
        LIGHT_BROWN = 14,
        WHITE = 15,
    };
    void init();
    void newline();
    void move(size_t x,size_t y);
    void move(size_t pos);
    void setchar(char c);
    void write_c(char c);
    void write_s(const char * str);
    void write_s_skip_space(const char * str);
    void write_i(unsigned int i);
    void write_h(uint64_t h);//write int as hex
    void write_mem(uint64_t mem,int offset=0);//write as memory size
    void scroll(int len);
    void setcolor(color bg,color fg);
    void setbgcolor(color c);
    void setfgcolor(color c);
    color getbgcolor();
    color getfgcolor();
    void clear();
    void clear_line(size_t line);
    size_t getX();
    size_t getY();
}

#endif // SCREEN_H_INCLUDED
