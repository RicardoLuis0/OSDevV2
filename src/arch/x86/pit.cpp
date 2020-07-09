#include "arch/x86.h"
#include "klib.h"
#include "screen.h"
#include <stdatomic.h>

using namespace PIT;

constexpr uint64_t ms_to_hz(uint64_t ms){
    return 1000/ms;
}

constexpr uint16_t div_hz(uint64_t hz){
    return (1193181ull/hz)&0xffff;
}

volatile atomic_int_least64_t PIT::timer;
const uint32_t PIT::timer_resolution=1;//1ms PIT timer

void pit_interrupt(){
    PIT::timer++;
}

void PIT::init(){
    constexpr uint16_t freq=div_hz(ms_to_hz(timer_resolution));
    Screen::write_s("\n -Initializing PIT...");
    Screen::write_s("\n  .PIT frequency = ");
    Screen::write_i(timer_resolution);
    Screen::write_s("ms");
    IDT::set_irq_handler(0x20,pit_interrupt,IDT::G_32_INT,IDT::RING_0);
    outb(0x43, 0x36);
    outb(0x40,freq&0xFF);
    outb(0x40,(freq>>8)&0xFF);
    IDT::pic_enable(0);
    Screen::write_s("\n  .PIT ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}
