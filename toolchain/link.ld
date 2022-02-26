MEMORY
{
	ram (rwx) : ORIGIN = 0xE00100, LENGTH = 0x0FFF00
}

STACK_SIZE = 0x2000;

start = 0xE00100;
ENTRY(start);

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors .vectors.*))
		*(.text*)
		*(.rodata*)
	} > ram

	.bss (NOLOAD) :
	{
		*(.bss*)
		*(COMMON)
	} > ram

	.data :
	{
		*(.data*);
	} > ram

	.stack (NOLOAD):
	{
		. = ALIGN(8);
		. = . + STACK_SIZE;
		. = ALIGN(8);
	} > ram

	_end = . ;
}
