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
    IDT::disable_exception_handler(8);
    IDT::disable_exception_handler(14);//disable page fault,dobule fault exception handlers and triple fault
    *reinterpret_cast<volatile uint32_t*>(Memory::next_free_virt_page(1))=0;
    k_abort_s("failed to reboot");
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

