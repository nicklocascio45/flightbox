ENTRY(Reset_Handler);

MEMORY {
	FLASH(rx)	: ORIGIN = 0x08000000, LENGTH = 512K
	SRAM(rwx)	: ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS {
    .vectors	: { KEEP(*(.vectors)) }		> FLASH
    .text		: { *(.text*) }				> FLASH
    .rodata		: { *(.rodata*) }			> FLASH

	.data : {
		_sdata = .;
		*(.data SORT(.data.*))
		_edata = .;
	} > SRAM AT > FLASH

	_la_data = LOADADDR(.data);

	.bss : {
		_sbss = .;
		*(.bss SORT(.bss.*) COMMON)
		_ebss = .;
	} > SRAM

	. = ALIGN(8);
}
