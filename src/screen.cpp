#include "screen.h"
#include "util.h"
#include "default/stdint.h"
#include "klib.h"

extern "C" void k_putc(char c){
    Screen::write_c(c);
}

extern "C" void k_puts(const char * s){
    Screen::write_s(s);
}

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
    if(ypos!=YMAX){
        ypos++;
    }else{
        scroll(1);
    }
}

void Screen::scroll(int len){
    if(len>0){
        uint32_t scr=len*XLEN;
        memmove((void*)vga,(void*)(vga+scr),POSLEN-scr);
    }else{
        //TODO scroll down
    }
}

void Screen::init(){
    vga=(uint16_t*) 0xB8000;
    clear();
    bg=BLACK;
    fg=WHITE;
    Screen::write_s(" -Initializing Console...\n> Console ");
    fg=LIGHT_GREEN;
    Screen::write_s("OK\n");
    fg=WHITE;
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
        newline();
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
            xpos=pos%XLEN;
            ypos=pos/XLEN;
            newline();
            pos=vga_xy(xpos,ypos);
            str++;
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

void Screen::write_s_skip_space(const char * str){
    int pos=vga_xy(xpos,ypos);
    while(*str!='\0'&&pos<POSLEN){
        switch(*str){
        case '\r'://carriage return
            str++;
            pos=((pos/XLEN)+1);
            break;
        case '\n'://newline
            xpos=pos%XLEN;
            ypos=pos/XLEN;
            newline();
            pos=vga_xy(xpos,ypos);
            str++;
            break;
        case '\b'://back
            str++;
            if(pos>0)pos--;
            break;
        case ' ':
            pos++;
            str++;
            break;
        default:
            vga[pos++]=vga_entry(*str++);
            break;
        }
    }
    move(pos);
}

static void write_i_rec(unsigned int i,int &pos){
    if(i>9){
        write_i_rec(i/10,pos);
        i%=10;
    }
    vga[pos++]=vga_entry(i+'0');
}

void Screen::write_i(unsigned int i){
    int pos=vga_xy(xpos,ypos);
    write_i_rec(i,pos);
    move(pos);
}

static void write_h_rec(uint64_t i,int &pos){
    if(i>15){
        write_h_rec(i/16,pos);
        i%=16;
    }
    vga[pos++]=vga_entry((i<10)?(i+'0'):((i-10)+'A'));
}

void Screen::write_h(uint64_t h){
    write_s("0x");
    int pos=vga_xy(xpos,ypos);
    write_h_rec(h,pos);
    move(pos);
}

void Screen::write_mem(uint64_t mem,int depth){
    char id;
    switch(depth){
    case 0:
        id='B';
        break;
    case 1:
        id='K';
        break;
    case 2:
        id='M';
        break;
    case 3:
        id='G';
        break;
    case 4:
        id='T';
        break;
    default:
        id='?';
        break;
    }
    bool last=false;
    if(mem>=1024ULL){
        write_mem(mem/1024ULL,depth+1);
    }else{
        last=true;
    }
    mem%=1024ULL;
    if(mem!=0||last){
        if(!last){
            write_c(' ');//whitespace between numbers
        }
        write_i(mem%1024ULL);
        write_c(id);
    }
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

void Screen::setcolor(color bg_c,color fg_c){
    bg=bg_c;
    fg=fg_c;
}

color Screen::getbgcolor(){
    return bg;
}

color Screen::getfgcolor(){
    return fg;
}
