#include "arch/x86.h"
#include "acpi.h"
#include "klib.h"

void PCI::init(){
    //TODO
}

//https://wiki.osdev.org/PCI

static constexpr uint32_t addr(uint8_t bus, uint8_t slot, uint8_t func,uint16_t off){
    return static_cast<uint32_t>((static_cast<uint32_t>(bus) << 16) | (static_cast<uint32_t>(slot) << 11) | (static_cast<uint32_t>(func) << 8) | (off & 0xfc) | (static_cast<uint32_t>(0x80000000)));
}

void PCI::writeb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint8_t val){
    outl(0xcf8,addr(bus,slot,func,off));
    outb(0xcfc+(off&3),val);
}

void PCI::writew(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint16_t val){
    outl(0xcf8,addr(bus,slot,func,off));
    outw(0xcfc+(off&3),val);
}

void PCI::writed(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint32_t val){
    outl(0xcf8,addr(bus,slot,func,off));
    outl(0xcfc+(off&3),val);
}

uint8_t PCI::readb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    outl(0xcf8,addr(bus,slot,func,off));
    return inb(0xcfc+(off&3));
}

uint16_t PCI::readw(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    outl(0xcf8,addr(bus,slot,func,off));
    return inw(0xcfc+(off&3));
}

uint32_t PCI::readd(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    outl(0xcf8,addr(bus,slot,func,off));
    return inl(0xcfc+(off&3));
}
