#include "screen.h"
#include "util.h"
#include "stdc/stdint.h"
#include "klib.h"
#include "arch/x86.h"

extern "C" {

    void putc(char c){
        Screen::write_c(c);
    }

    void puts(const char * s){
        Screen::write_s(s);
    }

    void k_putc(char c){
        Screen::write_c(c);
    }

    void k_puts(const char * s){
        Screen::write_s(s);
    }

    void k_putsn(const char * s,size_t n){
        Screen::write_sn(s,n);
    }

    void k_cls(){
        Screen::clear();
    }

    void k_puti(int32_t i){
        Screen::write_i(i);
    }

    void k_putu(int32_t u){
        Screen::write_u(u);
    }

    void k_putll(int64_t ll){
        Screen::write_ll(ll);
    }

    void k_putull(int64_t ull){
        Screen::write_ull(ull);
    }

    void k_puth(uint64_t h){
        Screen::write_h(h);
    }

    void k_putmem(uint64_t mem){
        Screen::write_mem(mem,0);
    }

    size_t k_get_y(){
        return Screen::getY();
    }

    size_t k_get_x(){
        return Screen::getX();
    }

    void k_set_xy(size_t x,size_t y){
        Screen::move(x,y);
    }

}

using namespace Screen;

size_t xpos;
size_t ypos;
color bg;
color fg;
uint8_t color_value;

constexpr size_t XMAX=79;
constexpr size_t YMAX=24;

constexpr size_t XLEN=80;
constexpr size_t YLEN=25;

constexpr size_t POSLEN=XLEN*YLEN;
constexpr size_t POSMAX=(XLEN*YLEN)-1;

volatile uint16_t * vga;

static inline void update_color() {
    color_value=(fg|(bg<<4));
}

static inline uint16_t vga_entry(uint8_t uc) {
    return (((uint16_t) uc) | ((uint16_t) (color_value << 8)));
}

constexpr size_t vga_xy(size_t x,size_t y){
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

void Screen::enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void Screen::disable_cursor(){
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void Screen::clear_line_multi(size_t linestart,size_t lineend){
    size_t start_pos=linestart*XLEN;
    size_t end_pos=lineend*XLEN;
    for(size_t pos=start_pos;pos<end_pos;pos++){
        vga[pos]=vga_entry(' ');
    }
}

void Screen::scroll(int len){
    if(len>0){
        uint32_t scr=len*XLEN;
        memcpy((void*)vga,(void*)(vga+scr),(POSLEN-scr)*2);
        clear_line_multi(25-len,25);
    }else if(len<0){
        //TODO scroll down
    }
}

void Screen::x86_init(){
    vga=(uint16_t*) 0xB8000;
    setcolor(BLACK,WHITE);//white on black color by default
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= ((uint16_t)inb(0x3D5)) << 8;
    move(pos);
}

void Screen::clear(){
    for(size_t pos=0;pos<POSLEN;pos++){
        vga[pos]=vga_entry(' ');
    }
    move(0,0);
}

void Screen::clear_line(size_t line){
    size_t start_pos=line*XLEN;
    size_t end_pos=(line+1)*XLEN;
    for(size_t pos=start_pos;pos<end_pos;pos++){
        vga[pos]=vga_entry(' ');
    }
    move(start_pos);
}

static void update_cursor(){
    uint16_t pos=vga_xy(xpos,ypos);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void Screen::move(size_t x,size_t y){
    xpos=clamp(0u,x,XMAX);
    ypos=clamp(0u,y,YMAX);
    update_cursor();
}

void Screen::move(size_t pos){
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

static int calcx(int pos){
    if(pos>POSMAX)pos=POSMAX;
    return pos%XLEN;
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
        case '\t':
            str++;
            vga[pos++]=vga_entry(' ');
            while(calcx(pos)%4){//4-space tabs
                vga[pos++]=vga_entry(' ');
            }
            break;
        default:
            if(pos>POSMAX)pos=POSMAX;
            vga[pos++]=vga_entry(*str++);
            break;
        }
    }
    move(pos);
}

void Screen::write_sn(const char * str,size_t n){
    int pos=vga_xy(xpos,ypos);
    size_t i=0;
    while(*str!='\0'&&pos<POSLEN&&i<n){
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
        case '\t':
            str++;
            while(calcx(pos)%4){//4-space tabs
                pos++;
            }
            break;
        default:
            vga[pos++]=vga_entry(*str++);
            break;
        }
        i++;
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

void Screen::write_i(int i){
    if(i<0){
        Screen::write_c('-');
        i*=-1;
    }
    Screen::write_u(i);
}

static void write_u_rec(unsigned int u,int &pos){
    if(u>9){
        write_u_rec(u/10,pos);
        u%=10;
    }
    vga[pos++]=vga_entry(u+'0');
}

void Screen::write_u(unsigned int u){
    int pos=vga_xy(xpos,ypos);
    write_u_rec(u,pos);
    move(pos);
}

void Screen::write_ll(int64_t ll){
    if(ll<0){
        Screen::write_c('-');
        ll*=-1;
    }
    Screen::write_ull(ll);
}

static void write_ull_rec(uint64_t u,int &pos){
    if(u>9){
        write_ull_rec(u/10,pos);
        u%=10;
    }
    vga[pos++]=vga_entry(u+'0');
}

void Screen::write_ull(uint64_t u){
    int pos=vga_xy(xpos,ypos);
    write_ull_rec(u,pos);
    move(pos);
}


static void write_h_rec(uint64_t h,int &pos){
    if(h>15){
        write_h_rec(h/16,pos);
        h%=16;
    }
    vga[pos++]=vga_entry((h<10)?(h+'0'):((h-10)+'A'));
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
        write_u(mem%1024ULL);
        write_c(id);
    }
}

void Screen::setchar(char c){
    vga[vga_xy(xpos,ypos)]=vga_entry(c);
}

void Screen::setbgcolor(color c){
    bg=c;
    update_color();
}

void Screen::setfgcolor(color c){
    fg=c;
    update_color();
}

void Screen::setcolor(color bg_c,color fg_c){
    bg=bg_c;
    fg=fg_c;
    update_color();
}

color Screen::getbgcolor(){
    return bg;
}

color Screen::getfgcolor(){
    return fg;
}

size_t Screen::getX(){
    return xpos;
}

size_t Screen::getY(){
    return ypos;
}
