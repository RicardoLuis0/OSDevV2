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

constexpr int POSLEN=XLEN*YLEN;
constexpr int POSMAX=(XLEN*YLEN)-1;

volatile uint16_t * vga;

static inline uint16_t vga_entry(uint8_t uc) {
    return (((uint16_t) uc) | ((uint16_t) (fg | (bg << 4)) << 8));
}

constexpr int vga_xy(int x,int y){
    return y * XLEN + x;
}

void Screen::newline(){
    xpos=0;
    if(ypos!=YLEN){
        ypos++;
    }
}

void Screen::init(){
    vga=(uint16_t*) 0xB8000;
    bg=BLACK;
    fg=WHITE;
    clear();
}

void Screen::clear(){
    for(int pos=0;pos<POSLEN;pos++){
        vga[pos]=vga_entry(' ');
    }
    move(0,0);
}

static void update_cursor(){
    uint16_t pos=vga_xy(xpos,ypos);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void Screen::move(int x,int y){
    xpos=clamp(0,x,XMAX);
    ypos=clamp(0,y,YMAX);
    update_cursor();
}

void Screen::move(int pos){
    if(pos>POSMAX)pos=POSMAX;
    xpos=pos%XLEN;
    ypos=pos/XLEN;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void Screen::write_c(char c){
    switch(c){
    case '\r'://carriage return
        xpos=0;
        break;
    case '\n'://newline
        xpos=0;
        if(ypos!=YLEN){
            ypos++;
        }
        break;
    case '\b'://back
        if(xpos>0){
            xpos--;
        }else{
            if(ypos>0){
                ypos--;
            }
        }
        break;
    default:
        vga[vga_xy(xpos,ypos)]=vga_entry(c);
        xpos++;
        if(xpos==XLEN){
            xpos=0;
            if(ypos!=YLEN){
                ypos++;
            }
        }
        break;
    }
    update_cursor();
}

void Screen::write_s(const char * str){
    int pos=vga_xy(xpos,ypos);
    while(*str!='\0'&&pos<POSLEN){
        switch(*str){
        case '\r'://carriage return
            str++;
            pos=((pos/XLEN)+1);
            break;
        case '\n'://newline
            str++;
            pos=((pos/XLEN)+1)*XLEN;
            break;
        case '\b'://back
            str++;
            if(pos>0)pos--;
            break;
        default:
            vga[pos++]=vga_entry(*str++);
            break;
        }
    }
    move(pos);
}

static void write_rec(unsigned int i,int &pos){
    if(i<10){
        vga[pos++]=vga_entry(i+'0');
    }else{
        write_rec(i/10,pos);
        vga[pos++]=vga_entry((i%10)+'0');
    }
}

void Screen::write_i(unsigned int i){
    int pos=vga_xy(xpos,ypos);
    write_rec(i,pos);
    move(pos);
}

void Screen::setchar(char c){
    vga[vga_xy(xpos,ypos)]=vga_entry(c);
}

void Screen::setbgcolor(color c){
    bg=c;
}

void Screen::setfgcolor(color c){
    fg=c;
}
