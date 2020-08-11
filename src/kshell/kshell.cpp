#include "kshell/kshell.h"
#include "klib.h"
#include "screen.h"
#include "mem.h"
#include "drivers/keyboard/ps2/keyboard.h"
#include "util/hash_table.h"
#include "arch/x86.h"
#include "acpi.h"
#include "util/smart_ptr.h"
#include "util/string.h"
#include "print.h"
#include <stdio.h>

struct kshell_cmd;

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

static constexpr char escape_char(char c){
    switch(c){
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 'e':
        return '\e';
    case 'f':
        return '\f';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'v':
        return '\v';
    default:
        return c;
    }
}

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

static void cmd_kbdump(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Drivers::Keyboard::PS2::cmd_kbdump();
}

static void cmd_meminfo(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Memory::cmd_meminfo();
}

static void cmd_help(char * cmd,Util::HashTable<kshell_cmd> * commands){
    char * arg=strchr(cmd,' ');
    if(arg){
        arg++;
        char * end=strchr(arg,' ');
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
        commands->foreach_v([](kshell_cmd &h_cmd){
            Screen::write_c('\n');
            Screen::write_s(h_cmd.name);
            Screen::write_s(" - ");
            Screen::write_s(h_cmd.description);
        });
    }
}

static void cmd_cpuid(char * cmd,Util::HashTable<kshell_cmd> * commands){
    CPUID::cmd_cpuid();
}


void cmd_parse(Util::Vector<Util::UniquePtr<char>> &data,const char * cmd){
    size_t sz=0;
    const char * start=cmd;
    bool reading_string=false;
    bool reading_escape=false;
    Util::String str;
    for(size_t i=0;cmd[i]!='\0';i++){
        char c=cmd[i];
        if(reading_escape){
            str+=escape_char(c);
            reading_escape=false;
        }else if(reading_string){
            if(c=='"'){
                data.push(str.release());
                reading_string=false;
            }else if(c=='\\'){
                reading_escape=true;
            }else{
                str+=c;
            }
        }else{
            if(c==' '||c=='"'){
                if(sz>0){
                    data.push(strndup(start,sz));
                    sz=0;
                }
                if(c=='"'){
                    reading_string=true;
                    reading_escape=false;
                }
            }else{
                if(sz==0){
                    start=cmd+i;
                }
                sz++;
            }
        }
    }
    if(sz>0){
        data.push(strndup(start,sz));
    }
}

extern "C" int lua_main (int argc, char **argv);

static void cmd_lua(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Util::Vector<Util::UniquePtr<char>> args;
    cmd_parse(args,cmd);
    k_putc('\n');
    lua_main(args.size(),(char**)args.get());
}

static Util::HashTable<kshell_cmd> * cmds=nullptr;

int kshell_execute(const char * cmd){
    if(!cmds)return 1;
    char * buf=strdup(cmd);
    char * temp=strchr(buf,' ');
    if(temp){
        *temp='\0';
    }
    if(cmds->has(buf)){
        kshell_cmd &kcmd=cmds->at(buf);
        if(temp){
            *temp=' ';
        }
        kcmd.cmd(buf,cmds);
        if(kcmd.cmd!=cmd_cls){
            Screen::write_c('\n');
        }
        free(buf);
        return 0;
    }else{
        free(buf);
        return 1;
    }
}

static void cmd_crdump(char * cmd,Util::HashTable<kshell_cmd> * commands){
    CR::cmd_crdump();
}

static void cmd_pagefault(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Memory::cmd_pagefault();
}

static void cmd_shutdown(char * cmd,Util::HashTable<kshell_cmd> * commands){
    ACPI::shutdown();
}

static void cmd_timer(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Screen::write_s("\ntimer = ");
    Screen::write_ull(PIT::timer);
}

static void cmd_fappendline(char * cmd,Util::HashTable<kshell_cmd> * commands){
    Util::Vector<Util::UniquePtr<char>> args;
    cmd_parse(args,cmd);
    if(args.size()<3){
        Screen::write_s("\nmissing arguments");
        return;
    }
    FILE * f=fopen(args[1],"a");
    if(!f){
        print("\nCould not open '",args[1].get(),"'");
        return;
    }
    fputs(args[2],f);
    fclose(f);
}

void kshell_init(){
    cmds=new Util::HashTable<kshell_cmd>();
    (*cmds)["cls"]={cmd_cls,"cls","clear the screen","- cls"};
    (*cmds)["halt"]={cmd_halt,"halt","halt the system","- halt"};
    (*cmds)["help"]={cmd_help,"help [command]","displays help","- help : list all commands\n- help [command] : show command usage"};
    (*cmds)["abort"]={cmd_abort,"abort","abort kernel","- abort"};
    (*cmds)["cpuid"]={cmd_cpuid,"cpuid","check cpu features","- cpuid"};
    (*cmds)["kbdump"]={cmd_kbdump,"kbdump","test keyboard mapping","- kbdump"};
    (*cmds)["meminfo"]={cmd_meminfo,"meminfo","display memory information","- meminfo"};
    (*cmds)["lua"]={cmd_lua,"lua","run lua interpreter","- lua"};
    (*cmds)["crdump"]={cmd_crdump,"crdump","dump contents of control registers","- crdump"};
    (*cmds)["pagefault"]={cmd_pagefault,"pagefault","cause page fault","- pagefault"};
    (*cmds)["timer"]={cmd_timer,"timer","value of PIT timer","- timer"};
    (*cmds)["shutdown"]={cmd_shutdown,"shutdown","ACPI Shutdown","- shutdown"};
    (*cmds)["fappendline"]={cmd_fappendline,"fappendline","append line to file","- fappendline file \"line\""};
}

void kshell(){
    Screen::enable_cursor(14,15);
    Screen::write_c('\n');
    constexpr char ptr_marker='#';
    char cmdbuf[80];
    uint8_t cmdsize=0;//0-77
    uint8_t ptrpos=0;//0-77
    uint8_t line=Screen::getY();
    memset(cmdbuf,0,sizeof(cmdbuf));
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
                    memmove(&cmdbuf[ptrpos-1],&cmdbuf[ptrpos],cmdsize-ptrpos);
                }
                ptrpos--;
                cmdsize--;
                cmdbuf[cmdsize]='\0';
            }
            break;
        case '\n':{
                if(kshell_execute(cmdbuf)){
                    cmd_invalid(cmdbuf,cmds);
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
                    memmove(&cmdbuf[ptrpos+1],&cmdbuf[ptrpos],cmdsize-ptrpos);
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

