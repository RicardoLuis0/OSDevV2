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
    void isr_irr_dump();
    enum dump_apic_info_mode{
        DUMP_APIC_INFO_LAPIC,
        DUMP_APIC_INFO_IOAPIC_ALL,
        DUMP_APIC_INFO_IOAPIC_NUM,
        DUMP_APIC_INFO_IOAPIC_REDIR,
        DUMP_APIC_INFO_IOAPIC_REDIR_COMPACT,
    };
    void dump_apic_info(dump_apic_info_mode mode,uint32_t num);
}

#endif // X86_APIC_H_INCLUDED
