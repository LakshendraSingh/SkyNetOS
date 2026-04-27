ENTRY(_start)
SECTIONS
{

    . = 0x100000; /* Multiboot prefers >= 1MB */
    .text : ALIGN(4096)
    {
        *(.multiboot)
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        *(.data)
    }

    .bss : ALIGN(4096)
    {
        *(COMMON)
        *(.bss)

    }

}