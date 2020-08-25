#include "arch/x86.h"
#include "klib.h"
#include "screen.h"


static bool sse;

void FPU::init(){
    Screen::write_s("\n -Initializing FPU...");
    if(!CPUID::has(0,CPUID::FEAT_EDX_FPU)&&!CR0::hasFlags(CR0::EM)&&CR0::hasFlags(CR0::ET)){
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("invalid FPU setup");
        return;
    }
    CR0::enableFlags(CR0::NE);
    CR0::enableFlags(CR0::MP);
    sse=CPUID::has(0,CPUID::FEAT_EDX_SSE);
    if(sse){
        CR4::enableFlags(CR4::OSFXSR);
        CR4::enableFlags(CR4::OSXMMEXCPT);
    }else{
        CR4::disableFlags(CR4::OSFXSR);
        CR4::disableFlags(CR4::OSXMMEXCPT);
    }
    asm volatile ("finit");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}
