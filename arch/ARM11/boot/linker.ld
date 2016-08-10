ENTRY(_start)

SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = orig;
    __start = .;
	.text.remap :
	{
	    KEEP(*(.text.boot))
		start_remap.o(.text)
	}
	. = . + kernel_base;
	__text_start = .;
    .text : AT(ADDR(.text) - kernel_base)
    {
        *(.text)
    }
    . = ALIGN(4096); /* align to page size */
    __text_end = .;
 
    __rodata_start = .;
    .rodata : AT(ADDR(.rodata) - kernel_base)
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    __data_start = .;
    .data : AT(ADDR(.data) - kernel_base)
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss : AT(ADDR(.bss) - kernel_base)
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
    __end = .;
}