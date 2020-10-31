#ifndef X86_ACPI_H_INCLUDED
#define X86_ACPI_H_INCLUDED

#include "acpispec/tables.h"
namespace ACPI {
    
    namespace MADT {
        
        enum { //MADT flags
            LEGACY_PIC=1,
        };
        
        struct Table {
            acpi_header_t header;
            uint32_t local_apic_address;
            uint32_t flags;
        }__attribute__((packed));
        struct Entry{
            enum{//MADT entry types
                PROCESSOR=0,//represents a processor's local apic
                IO_APIC=1,//I/O APIC
                ISO=2,//interrupt source override
                NMI=4,//non-maskable interrupt
                LAPIC_64=5,//64-bit address of local apic
            };
            uint8_t type;
            uint8_t length;
        }__attribute__((packed));
        struct ProcessorEntry {
            enum{//ProcessorEntry flags
                ENABLED=0,
                ONLINE=1,
            };
            Entry header;
            uint8_t proc_id;
            uint8_t apic_id;
            uint32_t flags;
        }__attribute__((packed));
        struct IOAPICEntry{
            Entry header;
            uint8_t id;
            uint8_t reserved;
            uint32_t address;
            uint32_t global_system_interrupt_base;
        }__attribute__((packed));
        struct ISOEntry {
            enum{//ISOEntry flags
                ACTIVE_LOW=2,
                LEVEL_TRIGGERED=8,
            };
            Entry header;
            uint8_t bus_source;
            uint8_t irq_source;
            uint32_t global_system_interrupt;
            uint16_t flags;
        }__attribute__((packed));
        struct NMIEntry {
            enum{//NMIEntry flags
                ACTIVE_LOW=2,
                LEVEL_TRIGGERED=8,
            };
            Entry header;
            uint8_t proc_id;
            uint16_t flags;
            uint8_t lint_n;//LINT0 or LINT1
        }__attribute__((packed));
        struct LAPIC64Entry{
            Entry header;
            uint16_t reserved;
            uint64_t address;
        }__attribute__((packed));
        extern Entry **entries;
        extern size_t entry_count;
        
        void init();
        
        uint8_t resolve_irq_source(uint8_t);
        void dump_iso();
    }
}
#endif // X86_ACPI_H_INCLUDED
