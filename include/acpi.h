#ifndef ACPI_H_INCLUDED
#define ACPI_H_INCLUDED

#include <stdint.h>

namespace ACPI{
    void init();
    void * map_table(uint32_t addr);
    void unmap_table(void *);
    //TODO
}
#endif // ACPI_H_INCLUDED
