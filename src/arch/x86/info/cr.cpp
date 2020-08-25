#include "arch/x86.h"
#include "screen.h"
#include "klib.h"

static const char * cr0_flags_str[32] {
    "PE         -- protected mode enable bit",
    "MP         -- monitor co-processor",
    "EM         -- x87 is present",
    "TS         -- x87 task switched",
    "ET         -- x87 type",
    "NE         -- x87 numeric error",
    FORC8("reserved   -- unused"),
    "reserved   -- unused",
    "reserved   -- unused",
    "WP         -- ring 0 write protect",
    "reserved   -- unused",
    "AM         -- ring 3 alignment check",
    FORC8("reserved   -- unused"),
    "reserved   -- unused",
    "reserved   -- unused",
    "NW         -- write-through caching disable bit",
    "CD         -- memory caching disable bit",
    "PG         -- paging enable bit",
};

static const char * cr4_flags_str[32] {
    "VME        -- V8086 mode virtual interrupt support",
    "PVI        -- protected mode virtual interrupt support",
    "TSD        -- RDTSC only in ring0?",
    "DE         -- debugging extensions enabled",
    "PSE        -- is Page Size Extension (page size 4MB) enabled?",
    "PAE        -- is Physical Address Extension enabled?",
    "MCE        -- enable machine check interrupts",
    "PGE        -- page global enabled",
    "PCE        -- RDPCM at any priviledge?",
    "OSFXSR     -- enable FXSAVE FXRSTOR",
    "OSXMMEXCPT -- enable unmasked SSE exceptions",
    "UMIP       -- prevent usermode access to SGDT, SIDT, SLDT, SMSW and STR",
    "LA57       -- enable 5-level paging",
    "VMXE       -- VT-x, Intel Virtualization Technology",
    "SMXE       -- Intel Trusted Execution Technology",
    "reserved   -- unused",
    "FSGSBASE   -- enable RDFSBASE RDGSBASE WRFSBASE WRGSBASE",
    "PCIDE      -- enable PCID",
    "OSXAVE     -- enable XSAVE",
    "reserved   -- unused",
    "SMEP       -- enable Supervisor Mode Execution Protection",
    "SMAP       -- enable Supervisor Mode Access Prevention",
    "PKE        -- enable Protection Key",
    FORC8("reserved   -- unused"),
    "reserved   -- unused",
};

void CR::cmd_crdump(){
    uint32_t lines=0;
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("\nCR0");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    uint32_t cr0=CR0::get();
    for(uint32_t i=0;i<32;i++){
        if(cr0&(1<<i)){
            if(lines==24){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=0;
            }
            lines++;
            k_putc('\n');
            k_puts(cr0_flags_str[i]);
        }
    }
    if(lines>20){
        k_puts("\npress any key to continue...");
        k_getch_extended();
        lines=0;
    }
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("CR3");
    Screen::setfgcolor(Screen::WHITE);
    k_putc('=');
    k_puth(CR3::get());
    k_putc('\n');
    Screen::setfgcolor(Screen::LIGHT_CYAN);
    k_puts("CR4");
    Screen::setfgcolor(Screen::WHITE);
    lines++;
    uint32_t cr4=CR4::get();
    for(uint32_t i=0;i<32;i++){
        if(cr4&(1<<i)){
            if(lines==24){
                k_puts("\npress any key to continue...");
                k_getch_extended();
                lines=0;
            }
            lines++;
            k_putc('\n');
            k_puts(cr4_flags_str[i]);
        }
    }
}

namespace CR0{
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

namespace CR2 {
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr2,%0":"=r"(t):);
        return t;
    }
}
namespace CR3 {
    uint32_t get(){
        uint32_t t;
        asm volatile("mov %%cr3,%0":"=r"(t):);
        return t;
    }
    void set(uint32_t t){
        asm volatile("mov %0,%%cr3"::"r"(t));
    }
}
namespace CR4 {
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
