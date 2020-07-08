#include "arch/x86.h"
#include "acpi.h"
#include "klib.h"

void PCI::init(){
    //TODO
}

void PCI::writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val){
    k_abort_s("PCI::writeb unimplemented");
}

void PCI::writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val){
    k_abort_s("PCI::writew unimplemented");
}

void PCI::writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val){
    k_abort_s("PCI::writed unimplemented");
}

uint8_t PCI::readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
    k_abort_s("PCI::readb unimplemented");
}

uint16_t PCI::readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
    k_abort_s("PCI::readw unimplemented");
}

uint32_t PCI::readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset){
    k_abort_s("PCI::readd unimplemented");
}
