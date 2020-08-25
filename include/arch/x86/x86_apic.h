#ifndef X86_APIC_H_INCLUDED
#define X86_APIC_H_INCLUDED

namespace APIC {
    void init();
    bool supported();
    void enable(uint8_t irq);
    void disable(uint8_t irq);
    uint8_t get_mapping(uint8_t irq);
}

#endif // X86_APIC_H_INCLUDED
