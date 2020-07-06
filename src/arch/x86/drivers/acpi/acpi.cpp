#include "drivers/acpi/acpi.h"
#include "klib.h"
#include "screen.h"

using namespace Drivers;
using namespace Drivers::ACPI;

void ACPI::init(){
    Screen::write_s("\n  .ACPI");
    Screen::write_c('\n');
    Screen::setcolor(Screen::RED,Screen::WHITE);
    Screen::clear_line(Screen::getY());
    Screen::write_s("   !ACPI unimplemented!");
    Screen::setcolor(Screen::BLACK,Screen::WHITE);
}
