#include "screen.h"
#include "util.h"
#include "default/stdint.h"

using namespace Screen;

int xpos;
int ypos;
color bg;
color fg;

constexpr int XMAX=79;
constexpr int YMAX=24;

constexpr int XLEN=80;
constexpr int YLEN=25;

volatile uint16_t * vga;

static inline uint16_t vga_entry(uint8_t uc) {
    return (((uint16_t) uc) | ((uint16_t) (fg | (bg << 4)) << 8));
}

constexpr int vga_xy(int x,int y){
    return y * XLEN + x;
}

void Screen::init(){
    vga=(uint16_t*) 0xB8000;
    bg=BLACK;
    fg=WHITE;
    clear();
}

void Screen::clear(){
    xpos=0;
    ypos=0;
    for(int pos=0;pos<XLEN*YLEN;pos++){
        vga[pos]=vga_entry(' ');
    }
}

void Screen::move(int x,int y){
    xpos=clamp(0,x,XMAX);
    ypos=clamp(0,y,YMAX);
}

void Screen::setchar(char c){
    vga[vga_xy(xpos,ypos)]=vga_entry(c);
}

void Screen::typechar(char c){
    vga[vga_xy(xpos,ypos)]=vga_entry(c);
    xpos++;
    if(xpos==XLEN){
        xpos=0;
        if(ypos!=YLEN){
            ypos++;
        }
    }
}

void Screen::setbgcolor(color c){
    bg=c;
}

void Screen::setfgcolor(color c){
    fg=c;
}
