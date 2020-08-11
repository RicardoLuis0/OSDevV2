#include "IOLayer.h"
#include "screen.h"
#include "klib.h"
#include "stdarg.h"
#include "stdio.h"

namespace TEdit::IOLayer {
    
    void init(){
        
    }
    
    keypress get_key(){
    retry:
        int kc=k_getch_extended();
        switch(kc){
        case K_GETCH_EXT_UP:
            return {0,ARROW_UP,1};
        case K_GETCH_EXT_DOWN:
            return {0,ARROW_DOWN,1};
        case K_GETCH_EXT_LEFT:
            return {0,ARROW_LEFT,1};
        case K_GETCH_EXT_RIGHT:
            return {0,ARROW_RIGHT,1};
        case K_GETCH_EXT_CTRL:
            goto retry;
        case K_GETCH_EXT_ALT:
            return {0,ALT,1};
        default:
            return {(char)kc,KEY,1};
        }
    }
    
    void moveCursor(int16_t x,int16_t y){
        Screen::move(x,y);
    }
    
    void writeChar(char c){
        k_putc(c);
    }
    
    void writeChars(const char * s,size_t n){
        k_putsn(s,n);
    }
    
    void writeStr(const char * s){
        k_puts(s);
    }
    
    void setColor(color fg,color bg){
        Screen::setcolor((Screen::color)bg,(Screen::color)fg);
    }
    
    void fillLine(int16_t line,int16_t rows,char c,color fg,color bg){
        Screen::color fg_tmp=Screen::getfgcolor();
        Screen::color bg_tmp=Screen::getbgcolor();
        setColor(fg,bg);
        Screen::fill_line_multi(line,line+rows,c);
        Screen::setcolor(bg_tmp,fg_tmp);
    }
    
    [[noreturn]] void exit_error(const char * fmt,...){
        va_list args1;
        va_start(args1,fmt);
        va_list args2;
        va_copy(args2,args1);
        size_t len=vsnprintf(NULL,0,fmt,args2);
        va_end(args2);
        char * buf=(char*)calloc(len+1,sizeof(char));
        vsnprintf(buf,len+1,fmt,args1);
        va_end(args1);
        fillLine(0,25,' ',WHITE,BLACK);
        moveCursor(0,0);
        k_abort_s(buf);
    }
}
