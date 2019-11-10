/*OUTPUT_FORMAT(binary)*/


/* https://wiki.osdev.org/Memory_Map_(x86) , stage 1,512b stack,stage 2 */
MEMORY {
	stage1 : o = 0x7C00, l = 512
	stage2 : o = 0x8000, l = 479K
}

SECTIONS {
	.stage1 : {
		*(.asm_stage1)
	} > stage1
	.stage2 : {
		*(.asm_stage2)
		*(.text)
		*(.data)
		*(.rodata)
		*(.bss)
	} > stage2
}