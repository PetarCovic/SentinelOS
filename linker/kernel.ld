ENTRY(kernel_start)

SECTIONS
{
    . = 0x9000;

    __kernel_start = .;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }

    . = ALIGN(4096);
    __kernel_end = .;
}