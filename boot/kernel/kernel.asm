[BITS 16]
[ORG 0x9000]

kernel_start:
    ;Setup segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;Setup stack
    cli
    mov ss, ax
    mov sp, 0x7C00
    sti

    ;Make sure string instructions move forward
    cld

    ;Print kernel message
    mov si, kernelMsg
    call print_string

hang:
    hlt
    jmp hang

kernelMsg:
    db "Kernel loaded", 13, 10, 0

%include "boot/bios/utils/print.asm"