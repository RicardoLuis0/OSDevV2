#ifndef ACPI_H_INCLUDED
#define ACPI_H_INCLUDED

#include <stdint.h>

namespace PCI {
    void init();
    void writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val);
    void writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val);
    void writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val);
    uint8_t readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset);
    uint16_t readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset);
    uint32_t readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset);
}

namespace ACPI {
    void init();
    void * map_table(uint32_t addr);
    void unmap_table(void *);
    //TODO
}
#endif // ACPI_H_INCLUDED
