#include "acpi.h"
#include "klib.h"
#include "screen.h"
#include "lai/helpers/pc-bios.h"

using namespace ACPI;

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
    
    //TODO MADT
    
    Screen::write_s("\n  .ACPI ");
    Screen::setfgcolor(Screen::RED);
    Screen::write_s("UNIMPLEMENTED");
    Screen::setfgcolor(Screen::WHITE);
}
