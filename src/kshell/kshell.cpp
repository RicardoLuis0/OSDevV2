#include "kshell/kshell.h"
#include "klib.h"
#include "screen.h"
#include "mem.h"
#include "drivers/keyboard/ps2/keyboard.h"
#include "util/hash_table.h"

void kshell_execute(char * cmd);

struct kshell_cmd;

static char * first_char(char * s,char c){
    while(*s!=c){
        if(!*s++)return 0;
    }
    return s;
}

/*unused
static bool is_int(char * s){
    size_t len=0;
    if(*s=='-')s++;
    while(*s>='0'&&*s<='9'){
        ++s;
        ++len;
    }
    return len>0&&*s=='\0';
}

static int parse_int(char * s){
    bool neg=false;
    if(*s=='-'){
        ++s;
        neg=true;
    }
    int num=0;
    while(*s>='0'&&*s<='9'){
        num*=10;
        num+=*s-'0';
        ++s;
    }
    return neg?-num:num;
}

static void fail_msg(const char * msg){
    Screen::write_c('\n');
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::clear_line(Screen::getY());
    Screen::write_s(msg);
    Screen::setcolor(Screen::BLACK,Screen::WHITE);
}
*/

static void cmd_invalid(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Screen::write_c('\n');
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::clear_line(Screen::getY());
    Screen::write_s("Unknown Command '");
    Screen::write_s(cmd);
    Screen::write_s("'");
    Screen::setcolor(Screen::BLACK,Screen::WHITE);
}

struct kshell_cmd{
    using kshell_cmd_t=void(*)(char*,Util::HashTable<kshell_cmd> *);
    kshell_cmd_t cmd;
    const char * name;
    const char * description;
    const char * usage;
    kshell_cmd():cmd(cmd_invalid),name("invalid"),description("invalid"),usage("invalid"){
    }
    kshell_cmd(kshell_cmd_t c,const char * n,const char * d,const char * u):cmd(c),name(n),description(d),usage(u){
    }
};

static void cmd_cls(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Screen::clear();
}

[[noreturn]] static void cmd_halt(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Screen::clear();
    k_halt();
}

static void cmd_abort(char * cmd,Util::HashTable<kshell_cmd> * commands){
    k_abort_fullscreen();
}

/*
static void cmd_kbdump(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Drivers::Keyboard::PS2::cmd_kbdump();
}
*/

static void cmd_meminfo(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Memory::cmd_meminfo();
}

static void cmd_help(char * cmd,Util::HashTable<kshell_cmd> * commands){
    char * arg=first_char(cmd,' ');
    if(arg){
        arg++;
        char * end=first_char(arg,' ');
        if(end){
            *end='\0';
        }
        if(commands->has(arg)){
            kshell_cmd &h_cmd=commands->at(arg);
            Screen::write_c('\n');
            Screen::write_s(h_cmd.name);
            Screen::write_s(" - ");
            Screen::write_s(h_cmd.description);
            Screen::write_s("\nusage:\n");
            Screen::write_s(h_cmd.usage);
        }else{
            cmd_invalid(arg,commands);
        }
    }else{
        commands->foreach([](kshell_cmd &h_cmd){
            Screen::write_c('\n');
            Screen::write_s(h_cmd.name);
            Screen::write_s(" - ");
            Screen::write_s(h_cmd.description);
        });
    }
}

void kshell(){
    Util::HashTable<kshell_cmd> commands;
    commands["cls"]={cmd_cls,"cls","clear the screen","- cls"};
    commands["halt"]={cmd_halt,"halt","halt the system","- halt"};
    commands["help"]={cmd_help,"help [command]","displays help","- help : list all commands\n- help [command] : show command usage"};
    commands["abort"]={cmd_abort,"abort","abort kernel","- abort"};
    //commands["kbdump"]={cmd_kbdump,"kbdump","test keyboard mapping","- kbdump"};
    commands["meminfo"]={cmd_meminfo,"meminfo","display memory information","- meminfo"};
    Screen::enable_cursor(14,15);
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
        case '\n':{
                char * temp=first_char(cmdbuf,' ');
                if(temp){
                    *temp='\0';
                }
                if(commands.has(cmdbuf)){
                    kshell_cmd &cmd=commands[cmdbuf];
                    if(temp){
                        *temp=' ';
                    }
                    cmd.cmd(cmdbuf,&commands);
                    if(cmd.cmd!=cmd_cls){
                        Screen::write_c('\n');
                    }
                }else{
                    cmd_invalid(cmdbuf,&commands);
                    Screen::write_c('\n');
                }
                line=Screen::getY();
                ptrpos=0;
                cmdsize=0;
                cmdbuf[cmdsize]='\0';
                break;
            }
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

