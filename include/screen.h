#ifndef SCREEN_H_INCLUDED
#define SCREEN_H_INCLUDED

#include "stdc/stdint.h"
#include "stdc/stddef.h"

namespace Screen {
    enum color {
        BLACK = 0,          //0b0000
        BLUE = 1,           //0b0001
        GREEN = 2,          //0b0010
        CYAN = 3,           //0b0011
        RED = 4,            //0b0100
        MAGENTA = 5,        //0b0101
        BROWN = 6,          //0b0110
        LIGHT_GREY = 7,     //0b0111
        DARK_GREY = 8,      //0b1000
        LIGHT_BLUE = 9,     //0b1001
        LIGHT_GREEN = 10,   //0b1010
        LIGHT_CYAN = 11,    //0b1011
        LIGHT_RED = 12,     //0b1100
        LIGHT_MAGENTA = 13, //0b1101
        LIGHT_BROWN = 14,   //0b1110
        WHITE = 15,         //0b1111
    };
    void newline();
    void move(int x,int y);
    void move(size_t pos);
    void setchar(char c);
    void write_c(char c);
    void write_s(const char * str);//FOR NOW ONLY THIS ONE SUPPORTS ANSI ESCAPE CODES
    void write_sn(const char * str,size_t n);
    void write_s_skip_space(const char * str);
    void write_i(int i);
    void write_u(unsigned int u);
    void write_ll(int64_t ll);
    void write_ull(uint64_t ull);
    void write_h(uint64_t h);//write int as hex
    void write_mem(uint64_t mem,int offset=0);//write as memory size
    void scroll(int len);
    void setcolor(color bg,color fg);
    void setbgcolor(color c);
    void setfgcolor(color c);
    void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
    void disable_cursor();
    color getbgcolor();
    color getfgcolor();
    void clear();
    void clear_line(size_t line);
    void fill_line(size_t line,char c);
    void clear_line_multi(size_t linestart,size_t lineend);
    void fill_line_multi(size_t linestart,size_t lineend,char c);
    size_t getX();
    size_t getY();
}

#endif // SCREEN_H_INCLUDED
