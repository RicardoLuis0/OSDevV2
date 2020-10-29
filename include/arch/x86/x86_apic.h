#ifndef X86_APIC_H_INCLUDED
#define X86_APIC_H_INCLUDED

namespace APIC {
    void init();
    bool supported();
    void enable(uint8_t irq);
    void disable(uint8_t irq);
    uint8_t get_mapping(uint8_t irq);
    void remap(uint8_t from,uint8_t to);
    bool has_remapping(uint8_t irq);
    void eoi();
}

#endif // X86_APIC_H_INCLUDED
