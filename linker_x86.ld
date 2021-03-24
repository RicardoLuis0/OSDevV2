ENTRY(_start)

SECTIONS {
    . = 1M ;
    kernel_start = . ;

    .rodata : ALIGN(4K) {
        *(.multiboot)
    }
    .text : ALIGN(4K) {
        *(.text)
        *(.init)
        *(.fini)
    }
    .rodata : ALIGN(4K) {
        *(.rodata)
    }
    .data : ALIGN(4K) {
        *(.data)
    }
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }
    kernel_end = . ;
}
