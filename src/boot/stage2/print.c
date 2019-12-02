#include "print.h"

void print(const char * c){
    asm(
        "mov %%ax,%%si\ncall asm_print_strtty"
        :
        :"a" (c)
    );
}

void printc(const char c){
    asm(
        "call asm_print_chartty"
        :
        :"a" (c)
    );
}

void move_xy(const unsigned char x,const unsigned char y){
    asm(
        "push %%ax\npush %%bx\ncall asm_move_xy"
        :
        :"a" (x)
        ,"b" (y)
    );
}

void print_xy(const char * c,const unsigned char x,const unsigned char y){
    move_xy(x,y);
    print(c);
}

void cls(){
    asm("call asm_cls");
}
