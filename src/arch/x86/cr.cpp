#include "arch/x86.h"
#include "screen.h"
#include "klib.h"

static const char * CR0_flags_str[32] {
    "CR0_PE -- protected mode enable bit",
    "CR0_MP -- monitor co-processor",
    "CR0_EM -- x87 is present",
    "CR0_TS -- x87 task switched",
    "CR0_ET -- x87 type",
    "CR0_NE -- x87 numeric error",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "CR0_WP -- ring 0 write protect",
    "reserved -- unused",
    "CR0_AM -- ring 3 alignment check",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "CR0_NW -- write-through caching disable bit",
    "CR0_CD -- memory caching disable bit",
    "CR0_PG -- paging enable bit",
};
static const char * CR4_flags_str[32] {
    "CR4_VME -- V8086 mode virtual interrupt support",
    "CR4_PVI -- protected mode virtual interrupt support",
    "CR4_TSD -- RDTSC only in ring0?",
    "CR4_DE -- debugging extensions enabled",
    "CR4_PSE -- is Page Size Extension (page size 4MB) enabled?",
    "CR4_PAE -- is Physical Address Extension enabled?",
    "CR4_MCE -- enable machine check interrupts",
    "CR4_PGE -- page global enabled",
    "CR4_PCE -- RDPCM at any priviledge?",
    "CR4_OSFXSR -- enable FXSAVE FXRSTOR",
    "CR4_OSXMMEXCPT -- enable unmasked SSE exceptions",
    "CR4_UMIP -- prevent usermode access to SGDT, SIDT, SLDT, SMSW and STR",
    "CR4_LA57 -- enable 5-level paging",
    "CR4_VMXE -- VT-x, Intel Virtualization Technology",
    "CR4_SMXE -- Intel Trusted Execution Technology",
    "reserved -- unused",
    "CR4_FSGSBASE -- enable RDFSBASE RDGSBASE WRFSBASE WRGSBASE",
    "CR4_PCIDE -- enable PCID",
    "CR4_OSXAVE -- enable XSAVE",
    "reserved -- unused",
    "CR4_SMEP -- enable Supervisor Mode Execution Protection",
    "CR4_SMAP -- enable Supervisor Mode Access Prevention",
    "CR4_PKE -- enable Protection Key",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
    "reserved -- unused",
};

void CR::cmd_crdump(){
    k_putc('\n');
    uint32_t lines=k_get_y();
    if(lines>20){
        k_cls();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("CR0\n");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    uint32_t cr0=CR0::get();
    for(uint32_t i=0;i<32;i++){
        if(cr0&(1<<i)){
            if(lines==24){
                k_puts("press any key to continue...");
                k_getch_extended();
                k_cls();
                lines=0;
            }
            lines++;
            k_puts(CR0_flags_str[i]);
            k_putc('\n');
        }
    }
    if(lines>20){
        k_puts("press any key to continue...");
        k_getch_extended();
        k_cls();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("CR3");
    Screen::setfgcolor(Screen::WHITE);
    k_putc('=');
    k_puth(CR3::get());
    k_putc('\n');
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("CR4\n");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    uint32_t cr4=CR4::get();
    for(uint32_t i=0;i<32;i++){
        if(cr4&(1<<i)){
            if(lines==24){
                k_puts("press any key to continue...");
                k_getch_extended();
                k_cls();
                lines=0;
            }
            lines++;
            k_puts(CR4_flags_str[i]);
            k_putc('\n');
        }
    }
}

namespace CR::CR0{
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr0,%0":"=r"(t):);
        return t;
    }
    static void set(uint32_t t){
        asm volatile("mov %0,%%cr0"::"r"(t));
    }
    void enableFlags(uint32_t flags){
        set(get()|flags);
    }
    void disableFlags(uint32_t flags){
        set(get()&~flags);
    }
    bool hasFlags(uint32_t flags){
        return ((flags&get())==flags);
    }
}

namespace CR::CR2 {
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr2,%0":"=r"(t):);
        return t;
    }
}
namespace CR::CR3 {
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr3,%0":"=r"(t):);
        return t;
    }
    void set(uint32_t t){
        asm volatile("mov %0,%%cr3"::"r"(t));
    }
}
namespace CR::CR4 {
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr4,%0":"=r"(t):);
        return t;
    }
    static void set(uint32_t t){
        asm volatile("mov %0,%%cr4"::"r"(t));
    }
    void enableFlags(uint32_t flags){
        set(get()|flags);
    }
    void disableFlags(uint32_t flags){
        set(get()&~flags);
    }
    bool hasFlags(uint32_t flags){
        return ((flags&get())==flags);
    }
}
