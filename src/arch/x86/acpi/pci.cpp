#include "arch/x86.h"
#include "acpi.h"
#include "klib.h"

void PCI::init(){
    //TODO
}

//https://wiki.osdev.org/PCI

static constexpr uint32_t addr(uint8_t bus, uint8_t slot, uint8_t func,uint16_t off){
    return (uint32_t)((((uint32_t)bus) << 16) | (((uint32_t)slot) << 11)|(((uint32_t)func) << 8) | (off & 0xfc) | ((uint32_t)0x80000000));
}

void PCI::writeb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint8_t val){
    outl(0xcf8,addr(bus,slot,func,off));
    //TODO ???
    k_abort_s("PCI::writeb unimplemented");
}

void PCI::writew(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint16_t val){
    k_abort_s("PCI::writew unimplemented");
}

void PCI::writed(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint32_t val){
    k_abort_s("PCI::writed unimplemented");
}

uint8_t PCI::readb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    k_abort_s("PCI::readb unimplemented");
}

uint16_t PCI::readw(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    k_abort_s("PCI::readw unimplemented");
}

uint32_t PCI::readd(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off){
    k_abort_s("PCI::readd unimplemented");
}
