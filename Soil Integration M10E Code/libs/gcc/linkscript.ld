MEMORY
{
	ROM (rx) :  ORIGIN = 0x08540000, LENGTH = 0x100000
	RAM (rwx) : ORIGIN = 0x00200000, LENGTH = 0x200000
}

SECTIONS
{
    . = 0x08540000;
	.initdata : 
	{ 
		KEEP(*(.initdata));
	} > ROM	
	text : 
	{ 
	    *(.text) 
	} > ROM
	.rodata : 
	{
	    *(.rodata)
	} > ROM
	.data : 
	{
		PROVIDE_HIDDEN (__data_load = LOADADDR(.data));
		PROVIDE_HIDDEN (__data_start = .);
		*(.data)
		PROVIDE_HIDDEN (_edata = .);
	} > RAM  AT > ROM
	.bss : 
	{
		PROVIDE_HIDDEN (__bss_start__ = .);
		*(.bss)
		PROVIDE_HIDDEN (__bss_end__ = .);
	} > RAM
}