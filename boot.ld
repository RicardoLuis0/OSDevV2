/*OUTPUT_FORMAT(binary)*/

MEMORY {
	boot : o = 0x7C00, l = 512
}

SECTIONS {
	.boot : { *(.boot) } > boot
	.text : { *(.text) }
	.data : { *(.data) }
	.bss  : { *(.bss)  }
}