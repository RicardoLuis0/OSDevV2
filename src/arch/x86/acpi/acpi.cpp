#include "acpi.h"
#include "klib.h"
#include "screen.h"
#include "lai/helpers/pc-bios.h"
#include "lai/helpers/pm.h"
#include "lai/helpers/sci.h"
#include "acpispec/tables.h"

using namespace ACPI;
using namespace ACPI::Internal;

acpi_rsdp_t * rsdp;
acpi_rsdt_t * rsdt;

acpi_xsdp_t * xsdp;
acpi_xsdt_t * xsdt;

acpi_fadt_t * fadt;

lai_rsdp_info rsdp_info;

void * ACPI::Internal::map_table(uint32_t addr){
    acpi_header_t * t=(acpi_header_t*)laihost_map(addr,sizeof(acpi_header_t));
    uint32_t len=t->length;
    laihost_unmap(t,sizeof(acpi_header_t));
    t=(acpi_header_t*)laihost_map(addr,len);
    return t;
}

void ACPI::Internal::unmap_table(void * p){
    acpi_header_t * t=(acpi_header_t*)p;
    laihost_unmap(t,t->length);
}

void ACPI::init(){
    Screen::write_s("\n -Initializing ACPI...");
    if(lai_bios_detect_rsdp(&rsdp_info)!=LAI_ERROR_NONE){
        Screen::write_s("\n  .ACPI ");
        Screen::setfgcolor(Screen::RED);
        Screen::write_s("FAIL");
        Screen::setfgcolor(Screen::WHITE);
        k_abort_s("ACPI Unavailable");
    }
    Screen::write_s("\n  .ACPI Version ");
    switch(rsdp_info.acpi_version){
    case 1:
        Screen::write_s("1.0");
        break;
    case 2:
        Screen::write_s("2.0");
        break;
    default:
        Screen::write_s("Unknown (");
        Screen::write_i(rsdp_info.acpi_version);
        Screen::write_s(")");
        break;
    }
    
    //setup initial tables
    
    rsdp=(acpi_rsdp_t*)map_table(rsdp_info.rsdp_address);
    rsdt=(acpi_rsdt_t*)map_table(rsdp_info.rsdt_address);
    if(rsdp_info.acpi_version>=2){
        xsdp=(acpi_xsdp_t*)rsdp;
        xsdt=(acpi_xsdt_t*)map_table(rsdp_info.xsdt_address);
    }
    
    fadt=(acpi_fadt_t*)laihost_scan("FACP",0);
    if(!fadt)k_abort_s("Couldn't find FADT header!");
    
    //initialize LAI
    
    lai_set_acpi_revision(rsdp->revision);
    lai_create_namespace();
    
    #if defined(DEBUG) && defined(K_LAI_DEBUG_EXTRA)
    lai_enable_tracing(LAI_TRACE_OP|LAI_TRACE_IO|LAI_TRACE_NS);
    #endif // K_LAI_DEBUG_EXTRA
    
    //TODO MADT
    
    PCI::init();
    
    //TODO ACPI STI interrupt
    
    lai_enable_acpi(0);//enable ACPI in legacy PIC mode
    
    Screen::write_s("\n  .ACPI ");
    Screen::setfgcolor(Screen::LIGHT_GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}

void ACPI::shutdown(){
    lai_enter_sleep(5);//acpi sleep state 5 is soft shutdown
}

