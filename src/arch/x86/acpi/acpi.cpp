#include "acpi.h"
#include "klib.h"
#include "screen.h"
#include "lai/helpers/pc-bios.h"
#include "lai/helpers/pm.h"
#include "lai/helpers/sci.h"
#include "arch/x86.h"
#include "util/vector.h"
#include "serial.h"

using namespace ACPI;
using namespace ACPI::Internal;

acpi_rsdp_t * rsdp;
acpi_rsdt_t * rsdt;

acpi_xsdp_t * xsdp;
acpi_xsdt_t * xsdt;

acpi_fadt_t * fadt;

MADT::Table * madt;

lai_rsdp_info rsdp_info;

[[maybe_unused]]
static void gas_outb(acpi_gas_t &reg,uint8_t data){
    if(reg.access_size!=0&&reg.access_size!=1){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        MMIO8(static_cast<uint32_t>(reg.base),0U)=data;
        return;
    case 1://Port IO
        outb(reg.base,data);
        return;
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}

[[maybe_unused]]
static void gas_outw(acpi_gas_t &reg,uint16_t data){
    if(reg.access_size!=0&&reg.access_size!=2){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        MMIO16(static_cast<uint32_t>(reg.base),0U)=data;
        return;
    case 1://Port IO
        outw(reg.base,data);
        return;
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}

[[maybe_unused]]
static void gas_outl(acpi_gas_t &reg,uint32_t data){
    if(reg.access_size!=0&&reg.access_size!=3){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        MMIO32(static_cast<uint32_t>(reg.base),0U)=data;
        return;
    case 1://Port IO
        outl(reg.base,data);
        return;
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}
[[maybe_unused]]
static uint8_t gas_inb(acpi_gas_t &reg){
    if(reg.access_size!=0&&reg.access_size!=1){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        return MMIO8(static_cast<uint32_t>(reg.base),0U);
    case 1://Port IO
        return inb(reg.base);
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}

[[maybe_unused]]
static uint16_t gas_inw(acpi_gas_t &reg){
    if(reg.access_size!=0&&reg.access_size!=2){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        return MMIO16(static_cast<uint32_t>(reg.base),0U);
    case 1://Port IO
        return inw(reg.base);
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}

[[maybe_unused]]
static uint32_t gas_inl(acpi_gas_t &reg){
    if(reg.access_size!=0&&reg.access_size!=3){
        k_abort_s("requested accesss size is invalid for GAS register");
    }
    switch(reg.address_space){
    case 0://MMIO
        return MMIO32(static_cast<uint32_t>(reg.base),0U);
    case 1://Port IO
        return inl(reg.base);
    default:
        k_abort_fmt("unimplemented GAS register type %x",reg.address_space);
    }
}

[[maybe_unused]]
static void gas_out(acpi_gas_t &reg,uint32_t data){
    switch(reg.access_size){
    case 0:
        k_abort_s("unkown GAS register access size");
    case 1:
        gas_outb(reg,data);
        break;
    case 2:
        gas_outw(reg,data);
        return;
    case 3:
        gas_outl(reg,data);
        return;
    default:
        k_abort_s("unimplemented GAS register access size");
    }
}

[[maybe_unused]]
static uint32_t gas_in(acpi_gas_t &reg){
    switch(reg.access_size){
    case 0:
        k_abort_s("unkown GAS register access size");
    case 1:
        return gas_inb(reg);
    case 2:
        return gas_inw(reg);
    case 3:
        return gas_inl(reg);
    default:
        k_abort_s("unimplemented GAS register access size");
    }
}

void * ACPI::Internal::map_table(uint32_t addr){
    acpi_header_t * t=reinterpret_cast<acpi_header_t*>(laihost_map(addr,sizeof(acpi_header_t)));
    uint32_t len=t->length;
    laihost_unmap(t,sizeof(acpi_header_t));
    t=reinterpret_cast<acpi_header_t*>(laihost_map(addr,len));
    return t;
}

void ACPI::Internal::unmap_table(void * p){
    acpi_header_t * t=reinterpret_cast<acpi_header_t*>(p);
    laihost_unmap(t,t->length);
}

void ACPI::init(){
    
    //TODO allow boot on systems without ACPI support
    
#ifdef LAI_HOST_IDENTITY_MAP
    Memory::Internal::map_virtual_page_unsafe(0xC0000,0xC0000,0x40000,true);//identity map PCI memory/etc
#endif // LAI_HOST_IDENTITY_MAP
    
    if(lai_bios_detect_rsdp(&rsdp_info)!=LAI_ERROR_NONE){
        Screen::write_s("\n -Initializing ACPI...");
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("ACPI Unavailable");
    }
    
    //setup initial tables
    
    rsdp=reinterpret_cast<acpi_rsdp_t*>(map_table(rsdp_info.rsdp_address));
    rsdt=reinterpret_cast<acpi_rsdt_t*>(map_table(rsdp_info.rsdt_address));
    if(rsdp_info.acpi_version>=2){
        xsdp=reinterpret_cast<acpi_xsdp_t*>(rsdp);
        xsdt=reinterpret_cast<acpi_xsdt_t*>(map_table(rsdp_info.xsdt_address));
    }
    
    fadt=reinterpret_cast<acpi_fadt_t*>(laihost_scan("FACP",0));
    if(!fadt)k_abort_s("Couldn't find FADT table!");
    
    //initialize LAI
    
    lai_set_acpi_revision(rsdp->revision);
    lai_create_namespace();
    
#if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
    lai_enable_tracing(LAI_TRACE_OP|LAI_TRACE_IO|LAI_TRACE_NS);
#endif // K_LAI_DEBUG_EXTRA
    
    madt=reinterpret_cast<MADT::Table*>(laihost_scan("APIC",0));
    
    if(!madt){
        Screen::write_s("\n -Initializing ACPI...");
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("Could't find MADT table");
    }
    
    MADT::init();
    
    PCI::init();
    
    //TODO handle ACPI STI interrupt
    
    Screen::write_s("\n -Initializing ACPI...");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}

void ACPI::enable(){
    Screen::write_s("\n -Entering ACPI Mode...");
    if(lai_enable_acpi(APIC::supported()?1:0)==0){//enable ACPI, returns 0 on success
        Screen::setfgcolor(Screen::LIGHT_GREEN);
        Screen::write_s("OK");
        Screen::setfgcolor(Screen::WHITE);
    }else{
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("Could not enter ACPI Mode");
    }
}

void ACPI::shutdown(){
    lai_enter_sleep(5);//acpi sleep state 5 is soft shutdown
}

void ACPI::reset(){
    gas_outb(fadt->reset_register,fadt->reset_command);
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::write_s("\nACPI Reset Failed, Triple Faulting!");
    Screen::setcolor(Screen::BLACK,Screen::WHITE);
    IDT::disable_exception_handler(8);
    IDT::disable_exception_handler(14);//disable page fault,dobule fault exception handlers and triple fault
    *reinterpret_cast<volatile uint32_t*>(Memory::next_free_virt_page(1))=0;
    k_abort_s("Triple Fault Failed (?!?!?!)");
}

MADT::Entry ** MADT::entries;
size_t MADT::entry_count;


void MADT::init(){
    Util::Vector<Entry*> entry_vec;
    Entry * current=reinterpret_cast<Entry*>(madt+1);//pointer of just past madt, start of entries
    while(reinterpret_cast<uint32_t>(current)<(reinterpret_cast<uint32_t>(madt)+madt->header.length)){//don't read past end
        entry_vec.push_back(current);
        switch(current->type){
        #ifdef MADT_DEBUG
        case Entry::PROCESSOR:{
                ProcessorEntry * e=reinterpret_cast<ProcessorEntry *>(current);
                Serial::write_s("\nMADT Processor Entry");
                Serial::write_s("\n .Type:");
                Serial::write_h(e->header.type);
                Serial::write_s("\n .Length:");
                Serial::write_h(e->header.length);
                Serial::write_s("\n .Processor ID:");
                Serial::write_h(e->proc_id);
                Serial::write_s("\n .APIC ID:");
                Serial::write_h(e->apic_id);
                Serial::write_s("\n .Flags:");
                Serial::write_h(e->flags);
            }
            break;
        case Entry::IO_APIC:{
                IOAPICEntry * e=reinterpret_cast<IOAPICEntry *>(current);
                Serial::write_s("\nMADT I/O APIC Entry");
                Serial::write_s("\n .Type:");
                Serial::write_h(e->header.type);
                Serial::write_s("\n .Length:");
                Serial::write_h(e->header.length);
                Serial::write_s("\n .ID:");
                Serial::write_h(e->id);
                Serial::write_s("\n .Address:");
                Serial::write_h(e->address);
                Serial::write_s("\n .Global System Interrupt Base:");
                Serial::write_h(e->global_system_interrupt_base);
            }
            break;
        case Entry::ISO:{
                ISOEntry * e=reinterpret_cast<ISOEntry *>(current);
                Serial::write_s("\nMADT Interrupt Source Override Entry");
                Serial::write_s("\n .Type:");
                Serial::write_h(e->header.type);
                Serial::write_s("\n .Length:");
                Serial::write_h(e->header.length);
                Serial::write_s("\n .Bus Source:");
                Serial::write_h(e->bus_source);
                Serial::write_s("\n .IRQ Source:");
                Serial::write_h(e->irq_source);
                Serial::write_s("\n .Global System Interrupt:");
                Serial::write_h(e->global_system_interrupt);
                Serial::write_s("\n .Flags:");
                Serial::write_h(e->flags);
            }
            break;
        case Entry::NMI:{
                NMIEntry * e=reinterpret_cast<NMIEntry *>(current);
                Serial::write_s("\nMADT Non-Maskable Interrupt Entry");
                Serial::write_s("\n .Type:");
                Serial::write_h(e->header.type);
                Serial::write_s("\n .Length:");
                Serial::write_h(e->header.length);
                Serial::write_s("\n .Processor ID:");
                Serial::write_h(e->proc_id);
                Serial::write_s("\n .Flags:");
                Serial::write_h(e->flags);
                Serial::write_s("\n .LINT#n:");
                Serial::write_h(e->lint_n);
            }
            break;
        case Entry::LAPIC_64:{
                LAPIC64Entry * e=reinterpret_cast<LAPIC64Entry *>(current);
                Serial::write_s("\nMADT Found 64-bit Local Apic Address Override Entry");
                Serial::write_s("\n .Type:");
                Serial::write_h(e->header.type);
                Serial::write_s("\n .Length:");
                Serial::write_h(e->header.length);
                Serial::write_s("\n .Address:");
                Serial::write_h(e->address);
                break;
            }
        #else
        case Entry::PROCESSOR:
        case Entry::IO_APIC:
        case Entry::ISO:
        case Entry::NMI:
        case Entry::LAPIC_64:
            break;
        #endif // MADT_DEBUG
        default:
            k_abort_s("Unknown MADT entry Type");
        }
        current=reinterpret_cast<Entry*>(reinterpret_cast<uint8_t*>(current)+current->length);
    }
    entry_vec.shrink_to_fit();
    entries=entry_vec.release(entry_count);
}

uint8_t MADT::resolve_irq_source(uint8_t irq){
    for(uint32_t i=0;i<MADT::entry_count;i++){
        if(MADT::entries[i]->type==MADT::Entry::ISO){
            MADT::ISOEntry* entry=reinterpret_cast<MADT::ISOEntry*>(MADT::entries[i]);
            if(entry->irq_source==irq)return entry->global_system_interrupt;
        }
    }
    return irq;
}


