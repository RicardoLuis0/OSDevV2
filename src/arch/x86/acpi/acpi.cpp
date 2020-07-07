#include "acpi.h"
#include "klib.h"
#include "screen.h"
#include "lai/helpers/pc-bios.h"
#include "lai/helpers/sci.h"
#include "acpispec/tables.h"

using namespace ACPI;

acpi_rsdp_t * rsdp;
acpi_rsdt_t * rsdt;

acpi_xsdp_t * xsdp;
acpi_xsdt_t * xsdt;

lai_rsdp_info rsdp_info;

void ACPI::init(){
    Screen::write_s("\n -Initializing ACPI");
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
    
    //save tables
    rsdp=(acpi_rsdp_t*)laihost_map(rsdp_info.rsdp_address,sizeof(acpi_rsdp_t));
    rsdt=(acpi_rsdt_t*)laihost_map(rsdp->rsdt,sizeof(acpi_rsdt_t));
    if(rsdp_info.acpi_version>=2){
        xsdp=(acpi_xsdp_t*)rsdp;
        xsdt=(acpi_xsdt_t*)laihost_map(xsdp->xsdt,sizeof(acpi_xsdt_t));
    }
    
    //initialize LAI
    lai_set_acpi_revision(rsdp->revision);
    lai_create_namespace();
    
    //TODO MADT
    
    lai_enable_acpi(0);//enable ACPI in legacy PIC mode
    
    Screen::write_s("\n  .ACPI ");
    Screen::setfgcolor(Screen::GREEN);
    Screen::write_s("OK");
    Screen::setfgcolor(Screen::WHITE);
}
