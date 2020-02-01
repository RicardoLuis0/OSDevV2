#include "kshell/kshell.h"
#include "klib.h"
#include "screen.h"
#include "drivers/keyboard/ps2/keyboard.h"

void kshell_execute(char * cmd);

void kshell(){
    Screen::write_c('\n');
    constexpr char ptr_marker='#';
    char cmdbuf[80];
    uint8_t cmdsize=0;//0-77
    uint8_t ptrpos=0;//0-77
    uint8_t line=Screen::getY();
    k_memset(cmdbuf,0,sizeof(cmdbuf));
    for(;;){
        Screen::clear_line(line);
        Screen::setfgcolor(Screen::LIGHT_CYAN);
        Screen::write_c(ptr_marker);
        Screen::setfgcolor(Screen::WHITE);
        Screen::write_c(' ');
        Screen::write_s(cmdbuf);
        Screen::move(ptrpos+2,line);
        int c=k_getch_extended();
        switch(c){
        case K_GETCH_EXT_RIGHT:
            if(ptrpos<cmdsize)ptrpos++;
            break;
        case K_GETCH_EXT_LEFT:
            if(ptrpos>0)ptrpos--;
            break;
        case '\b':
            if(cmdsize>0){
                if(ptrpos!=cmdsize){
                    k_memmove(&cmdbuf[ptrpos-1],&cmdbuf[ptrpos],cmdsize-ptrpos);
                }
                ptrpos--;
                cmdsize--;
                cmdbuf[cmdsize]='\0';
            }
            break;
        case '\n':
            kshell_execute(cmdbuf);
            line=Screen::getY();
            ptrpos=0;
            cmdsize=0;
            cmdbuf[cmdsize]='\0';
            break;
        default:
            if(c<128&&cmdsize<77){
                if(ptrpos!=cmdsize){
                    k_memmove(&cmdbuf[ptrpos+1],&cmdbuf[ptrpos],cmdsize-ptrpos);
                }
                cmdbuf[ptrpos]=c;
                ptrpos++;
                cmdsize++;
                cmdbuf[cmdsize]='\0';
            }
            break;
        }
    }
}

void kshell_execute(char * cmd){
    if(k_strcmp_bool(cmd,"abort")){
        k_abort_fullscreen();
    }else if(k_strcmp_bool(cmd,"kbdump")){
        Drivers::Keyboard::PS2::kbdump();
    }else if(k_strcmp_bool(cmd,"cls")){
        Screen::clear();
    }else{
        Screen::write_c('\n');
        Screen::setcolor(Screen::RED,Screen::WHITE);
        Screen::clear_line(Screen::getY());
        Screen::write_s("Unknown Command '");
        Screen::write_s(cmd);
        Screen::write_s("'");
        Screen::setcolor(Screen::BLACK,Screen::WHITE);
    }
    Screen::write_c('\n');
}
