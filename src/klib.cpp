#include "klib.h"
#include "util.h"
#include "print.h"
#include "kshell/kshell.h"
#include "arch/x86.h"
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <util/hash_table.h>
#include <util/smart_ptr.h>

Util::HashTable<Util::UniquePtr<char>> * kernel_env=nullptr;

void k_env_init(){
    kernel_env=new Util::HashTable<Util::UniquePtr<char>>();
}

bool klib_init(){
    k_env_init();
    return true;
}

extern "C" {
    
    void k_sleep(uint64_t ms){
        uint64_t e=PIT::timer+(ms/PIT::timer_resolution);
        while(PIT::timer<e)asm("pause");
    }
    
    int errno;
    
    time_t time(time_t* timer){
        //k_abort_s("time unimplemented");
        return 0;
    }
    
    clock_t clock(void){
        return static_cast<clock_t>(-1);
    }
    
    time_t mktime(tm *time){
        k_abort_s("mktime unimplemented");
    }
    
    tm * gmtime(const time_t *timer){
        k_abort_s("gmtime unimplemented");
    }
    
    tm * localtime(const time_t *timer){
        k_abort_s("localtime unimplemented");
    }
    
    size_t strftime(char *s, size_t n,const char *fmt,const struct tm *){
        k_abort_s("strftime unimplemented");
    }
    
    double difftime(time_t time_end,time_t time_beg){
        k_abort_s("difftime unimplemented");
    }
    
    char * getenv(const char* name){
        auto * p=kernel_env->find(name);
        return p?p->get():NULL;
    }
    
    void (*signal( int sig, void (*handler) (int))) (int){
        return SIG_ERR;
    }

    int setenv(const char *name,const char *value,int overwrite){
        if(overwrite||!kernel_env->has(name)){
            (*kernel_env)[name]=strdup(value);
        }
        return 0;
    }
    
    int unsetenv(const char *name){
        kernel_env->unset(name);
        return 0;
    }
    
    int __cxa_atexit(void (*func) (void *), void * arg, void * d){
        k_abort_s("__cxa_atexit unimplemented");
    }
    
    [[noreturn]] void __cxa_pure_virtual(){
        k_abort_s("call to pure virtual method");
    }
    
    int atexit(void(*func)(void)){
        return __cxa_atexit(reinterpret_cast<void (*)(void*)>(func),NULL,NULL);
    }
    
    const char * err_unknown = "unknown error";
    char * errstr=const_cast<char*>(err_unknown);
    
    char * strerror(int errnum) {
        return errstr;
    }
    
    [[noreturn]] void abort() {
        k_abort();
    }
    
    [[noreturn]] void exit(int c) {
        char buf[256]={0};
        snprintf(buf,255,"Exit Called with Code %d",c);
        k_abort_s(buf);
    }
    
    int system(const char * cmd){
        return kshell_execute(cmd);
    }
    
    char * k_gets(char * buf, int n){
        //TODO FIX???
        buf[0]='\0';
        size_t y=k_get_y();
        size_t x=k_get_x();
        size_t i=0;
        size_t sz=0;
        size_t szmax=min<size_t>(n-2,78-x);
        while(true){
            k_set_xy(x,y);
            k_puts(buf);
            k_set_xy(x+i,y);
            int c=k_getch_extended();
            switch(c){
            case K_GETCH_EXT_RIGHT:
                if(i<sz){
                    i++;
                }
                break;
            case K_GETCH_EXT_LEFT:
                if(i>0){
                    i--;
                }
                break;
            case '\b':
                if(sz>0){
                    if(i!=sz){
                        memmove(&buf[i-1],&buf[i],sz-i);
                    }
                    i--;
                    sz--;
                    buf[sz]='\0';
                    k_set_xy(x+sz,y);
                    k_putc(' ');
                }
                break;
            default:
                if((isgraph(c)||c=='\n'||c==' ')&&sz<szmax){
                    if(i!=sz){
                        memmove(&buf[i+1],&buf[i],sz-i);
                    }
                    if(c!='\n')buf[i]=c;
                    i++;
                    sz++;
                    buf[sz]='\0';
                    if(c=='\n'){
                        k_putc('\n');
                        return buf;
                    }
                }else if(c=='\n'){
                    buf[szmax]='\n';
                    buf[szmax+1]='\0';
                    k_putc('\n');
                    return buf;
                }
                break;
            }
        }
    }
    
    [[noreturn]] void hang();
    
    [[noreturn]] void k_halt(){
        //TODO clean up system before hang
        hang();
    }
    
    [[noreturn]] static inline void k_abort_main(){
        Screen::disable_cursor();
        k_halt();
    }
    
    [[noreturn]] void k_abort(){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted!");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_fullscreen(){
        Screen::setcolor(Screen::BLACK,Screen::WHITE);
        Screen::clear();
        Screen::setcolor(Screen::RED,Screen::WHITE);
        Screen::clear_line(0);
        Screen::move(32,0);
        print("Kernel Aborted!");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_i(int code){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! Error code '",code,"'");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_s(const char * msg){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! ",msg,"");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_fmt(const char * fmt,...){
        char msg[256];
        va_list arg;
        va_start(arg,fmt);
        vsnprintf(msg,256,fmt,arg);
        va_end(arg);
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! ",msg,"");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_s_i_s(const char * s1,int i,const char * s2){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! ",s1,i,s2,"");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_assert(const char * condition,const char * name,uint32_t line){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.");
        k_abort_main();
    }
    
    [[noreturn]] void k_abort_massert(const char * condition,const char * msg,const char * name,uint32_t line){
        Screen::setcolor(Screen::RED,Screen::WHITE);
        print("\nKernel Aborted! In ",name,":",line,", Assertion ",condition," failed.\n",msg);
        k_abort_main();
    }
    
    int k_strcmp_bool(const char * s1,const char * s2){
        while(*s1||*s2){
            if(*s1++!=*s2++)return false;
        }
        return true;
    }
    
    //source http://www.cse.yorku.ca/~oz/hash.html
    size_t k_hash_s(const char * s){
        size_t hash = 5381;
        while(*s++)hash = ((hash << 5) + hash) + (static_cast<size_t>(*s)); /* hash * 33 + c */
        return hash;
    }
    
    size_t k_format_ull_str(char * str,size_t n,uint64_t u){
        size_t w=0;
        if(u>9){
            w=k_format_ull_str(str,n,u/10);
            u%=10;
        }
        if(w<n){
            str[w]=u+'0';
            w++;
            str[w]='\0';
        }
        return w;
    }
    
    size_t k_format_mem_str(char * s,size_t n,uint64_t m,size_t depth){
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
        size_t w=0;
        if(m>=1024ULL){
            w=k_format_mem_str(s,n,m/1024ULL,depth+1);
        }else{
            last=true;
        }
        m%=1024ULL;
        if(m!=0||last){
            if(!last){
                if(w<n){
                    s[w]=' ';
                    w++;
                    s[w]='\0';
                }
            }
            if(w<n){
                w+=k_format_ull_str(s+w,n-w,m);
            }
            if(w<n){
                s[w]=id;
                w++;
                s[w]='\0';
            }
        }
        return w;
    }
}
