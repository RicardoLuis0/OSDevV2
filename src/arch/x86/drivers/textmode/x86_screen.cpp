#include "screen.h"
#include "util.h"
#include "klib.h"
#include "arch/x86.h"
#include "serial.h"
#include "util/vector.h"
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

extern "C" {
    
    void puts(const char * s){
        Screen::write_s(s);
    }
    
    void k_putc(char c){
        Screen::write_c(c);
    }
    
    void k_puts(const char * s){
        Screen::write_s(s);
    }
    
    void k_logs(const char * s){
        Screen::write_s(s);
        Serial::write_s(s);
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
    color_value=(fg|((bg&0b111)<<4));
}

static inline uint16_t vga_entry(uint8_t uc) {
    return (static_cast<uint16_t>(uc) | static_cast<uint16_t>(color_value << 8));
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
    fill_line_multi(linestart,lineend,' ');
}


void Screen::fill_line_multi(size_t linestart,size_t lineend,char c){
    size_t start_pos=linestart*XLEN;
    size_t end_pos=lineend*XLEN;
    for(size_t pos=start_pos;pos<end_pos;pos++){
        vga[pos]=vga_entry(c);
    }
}

void Screen::scroll(int len){
    if(len>0){
        uint32_t scr=len*XLEN;
        memcpy(reinterpret_cast<void*>(const_cast<uint16_t*>(vga)),reinterpret_cast<void*>(const_cast<uint16_t*>(vga)+scr),(POSLEN-scr)*2);
        clear_line_multi(25-len,25);
    }else if(len<0){
        //TODO scroll down
    }
}

void Screen::x86_init(){
    vga=reinterpret_cast<uint16_t*>(0xB8000);
    setcolor(BLACK,WHITE);//white on black color by default
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= static_cast<uint16_t>(inb(0x3D5)) << 8;
    disable_cursor();
    move(pos);
}

void Screen::clear(){
    for(size_t pos=0;pos<POSLEN;pos++){
        vga[pos]=vga_entry(' ');
    }
    move(0,0);
}

void Screen::clear_line(size_t line){
    fill_line(line,' ');
}

void Screen::fill_line(size_t line,char c){
    size_t start_pos=line*XLEN;
    size_t end_pos=(line+1)*XLEN;
    for(size_t pos=start_pos;pos<end_pos;pos++){
        vga[pos]=vga_entry(c);
    }
    move(start_pos);
}

static void update_cursor(){
    move(vga_xy(xpos,ypos));
}

static void update_xy(int x,int y){
    xpos=x>0?clamp<size_t>(0u,x,XMAX):0;
    ypos=y>0?clamp<size_t>(0u,y,YMAX):0;
}


static void update_xy(size_t pos){
    xpos=pos%XLEN;
    ypos=pos/XLEN;
}

void Screen::move(int x,int y){
    update_xy(x,y);
    move(vga_xy(xpos,ypos));
}

void Screen::move(size_t pos){
    if(pos>POSMAX)pos=POSMAX;
    xpos=pos%XLEN;
    ypos=pos/XLEN;
    outb(0x3D4, 0x0F);
    outb(0x3D5, static_cast<uint8_t>(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, static_cast<uint8_t>((pos >> 8) & 0xFF));
}

void Screen::write_fmt(char * fmt,...){
    va_list arg;
    va_start(arg,fmt);
    va_list a2;
    va_copy(a2,arg);
    int n=vsnprintf(NULL,0,fmt,a2);
    va_end(a2);
    char * s=reinterpret_cast<char*>(calloc(n+1,sizeof(char)));
    vsnprintf(s,n,fmt,arg);
    va_end(arg);
    write_sn(s,n);
    free(s);
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

size_t ansi_saved_pos=0;

bool ansi_intensity=false;

static color ansi_color_map[]{
    BLACK,
    RED,
    GREEN,
    BROWN,
    BLUE,
    MAGENTA,
    CYAN,
    LIGHT_GREY,
};

void Screen::write_s(const char * str){
    int pos=vga_xy(xpos,ypos);
    while(*str!='\0'&&pos<POSLEN){
        switch(*str){
        case '\e':
            str++;
            if(*str=='['){
                str++;
                Util::Vector<int> parameters;
                int val=0;
                int size=0;
                while(*str!='\0'&&(*str==';'||(*str>='0'&&*str<='9'))){
                    if(*str==';'){
                        if(size==0){
                            parameters.push_back(-1);
                        }else{
                            parameters.push_back(val);
                            val=0;
                            size=0;
                        }
                    }else{
                        val*=10;
                        val+=*str-'0';
                        size++;
                    }
                    str++;
                }
                if(size!=0){
                    parameters.push_back(val);
                }
                int n=0,m=0;
                switch(*str){
                case 'J':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }
                    //TODO better 'J' support, n=0 is clear from cursor to end, n=1 is clear from cursor to beginning, n=0 is clear entire screen
                    clear();
                    break;
                case 'K':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }
                    //TODO better 'K' support, n=0 is clear from cursor to end, n=1 is clear from cursor to beginning, n=0 is clear entire line
                    clear_line(ypos);
                    break;
                case 's':
                    ansi_saved_pos=pos;
                    break;
                case 'u':
                    pos=ansi_saved_pos;
                    move(pos);
                    break;
                case 'A':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(xpos,ypos-n);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'B':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(xpos,ypos+n);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'C':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(xpos+n,ypos);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'D':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(xpos-n,ypos);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'E':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(0,ypos+n);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'F':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(0,ypos-n);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'G':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    update_xy(pos);
                    move(xpos,n);
                    pos=vga_xy(xpos,ypos);
                    break;
                case 'S':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    scroll(n);
                    break;
                case 'T':
                    if(parameters.size()>0){
                        n=parameters[0];
                    }else{
                        n=1;
                    }
                    scroll(-n);
                    break;
                case 'm':
                    n=parameters.size();
                    if(n==0){
                        ansi_intensity=false;
                        setcolor(BLACK,LIGHT_GREY);
                    }else{
                        for(int i=0;i<n;i++){
                            int v=parameters[i];
                            if(v==0){
                                ansi_intensity=false;
                                setcolor(BLACK,LIGHT_GREY);
                            }else if(v==1){
                                ansi_intensity=true;
                                setcolor(static_cast<color>(bg%8),static_cast<color>((fg%8)+8));
                            }else if(v>=30&&v<=37){
                                setcolor(bg,static_cast<color>(ansi_color_map[v-30]+(ansi_intensity?8:0)));
                            }else if(v>=40&&v<=47){
                                setcolor(ansi_color_map[(v-40)],fg);
                            }
                        }
                    }
                case 'f':
                case 'H':
                    if(parameters.size()>1){
                        n=parameters[0];
                        m=parameters[1];
                    }else if(parameters.size()==1){
                        n=parameters[0];
                        m=1;
                    }else{
                        n=1;
                        m=1;
                    }
                    if(n<1)n=1;
                    if(m<1)m=1;
                    move(n-1,m-1);
                    break;
                case '?':
                    str++;
                    while(*str!='\0'&&*str>='0'&&*str<='9'){
                        n*=10;
                        n+=*str-'0';
                        str++;
                    }
                    if(n==25){
                        if(*str=='h'){
                            enable_cursor(14,15);
                        }else if(*str=='l'){
                            disable_cursor();
                        }
                    }else if(n==1049){
                        //NOT SUPPORTED
                    }else if(n==2004){
                        //NOT SUPPORTED
                    }
                    
                default:
                    //UNKNOWN ANSI ESCAPE CODE
                    break;
                }
            }
            str++;
            break;
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
                if(pos<POSLEN)vga[pos]=vga_entry(' ');
                pos++;
            }
            if(pos>=POSMAX){
                scroll(1);
                pos=vga_xy(0,YMAX);
            }
            break;
        default:
            if(isgraph(*str)||*str==' '){
                if(pos==POSMAX){
                    scroll(1);
                    pos=vga_xy(0,YMAX);
                }
                vga[pos]=vga_entry(*str);
            }
            str++;
            pos++;
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
            vga[pos++]=vga_entry(' ');
            while(calcx(pos)%4){//4-space tabs
                vga[pos++]=vga_entry(' ');
            }
            break;
        default:
            if(isgraph(*str)||*str==' '){
                vga[pos]=vga_entry(*str);
            }
            str++;
            pos++;
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
        write_u(mem);
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
