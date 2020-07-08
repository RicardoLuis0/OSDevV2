#ifndef ACPI_H_INCLUDED
#define ACPI_H_INCLUDED

#include <stdint.h>

namespace PCI {
    void init();
    void writeb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint8_t val);
    void writew(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint16_t val);
    void writed(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off, uint32_t val);
    uint8_t readb(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off);
    uint16_t readw(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off);
    uint32_t readd(uint8_t bus, uint8_t slot, uint8_t func, uint16_t off);
}

namespace ACPI {
    void init();
    namespace Internal {
        void * map_table(uint32_t addr);
        void unmap_table(void *);
    }
    //TODO
}
#endif // ACPI_H_INCLUDED
