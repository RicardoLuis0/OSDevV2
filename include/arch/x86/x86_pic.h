#ifndef X86_PIC_H_INCLUDED
#define X86_PIC_H_INCLUDED

namespace PIC {
    void init();//init PIC with all interrupts masked
    void unmask(uint8_t irq);
    void mask(uint8_t irq);
    void reset(uint8_t off1,uint8_t off2);//remap pic and mask all interrupts
    uint8_t get_mapping(uint8_t irq);
    void eoi();
}

#endif // X86_PIC_H_INCLUDED
