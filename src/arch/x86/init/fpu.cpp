#include "arch/x86.h"
#include "klib.h"
#include "screen.h"

using namespace CR;
using CR::CR0_EM,
      CR::CR0_ET,
      CR::CR0_NE,
      CPUID::CPUID_FEAT_EDX_FPU,
      CPUID::CPUID_FEAT_EDX_SSE;

static bool sse;

void FPU::init(){
    Screen::write_s("\n -Initializing FPU...");
    if(!CPUID::cpuid_has(0,CPUID_FEAT_EDX_FPU)&&!CR0::hasFlags(CR0_EM)&&CR0::hasFlags(CR0_ET)){
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("invalid FPU setup");
        return;
    }
    CR0::enableFlags(CR0_NE);
    CR0::enableFlags(CR0_MP);
    sse=CPUID::cpuid_has(0,CPUID_FEAT_EDX_SSE);
    if(sse){
        CR4::enableFlags(CR4_OSFXSR);
        CR4::enableFlags(CR4_OSXMMEXCPT);
    }else{
        CR4::disableFlags(CR4_OSFXSR);
        CR4::disableFlags(CR4_OSXMMEXCPT);
    }
    asm volatile ("finit");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}
