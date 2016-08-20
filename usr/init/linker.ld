ENTRY(_start)

SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = orig;
    _start = .;
	__text_start = .;
    .text : AT(ADDR(.text))
    {
        *(.text)
    }
    . = ALIGN(4096); /* align to page size */
    __text_end = .;
 
    __rodata_start = .;
    .rodata : AT(ADDR(.rodata))
    {
        *(.rodata)
    }
    . = ALIGN(4096); /* align to page size */
    __rodata_end = .;
 
    __data_start = .;
    .data : AT(ADDR(.data))
    {
        *(.data)
    }
    . = ALIGN(4096); /* align to page size */
    __data_end = .;
 
    __bss_start = .;
    .bss : AT(ADDR(.bss))
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096); /* align to page size */
    __bss_end = .;
	
    __end = .;
}