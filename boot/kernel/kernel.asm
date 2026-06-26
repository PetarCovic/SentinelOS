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
    call print_string16

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:init_pm

hang16:
    hlt
    jmp hang16

kernelMsg:
    db "Kernel Loaded", 13, 10, 0

%include "boot/bios/utils/print16.asm"

gdt_start:
    ;Null descriptor
    dw 0, 0, 0, 0

gdt_code:
    dw 0xFFFF, 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end-gdt_start-1
    dd gdt_start

CODE_SEG equ gdt_code-gdt_start
DATA_SEG equ gdt_data-gdt_start

[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000
    
    cld

    call kernel_main

hang32:
    hlt
    jmp hang32

protectedModeMsg:
    db "Protected Mode Reached", 0

kernel_main:
    ;Print protected mode reached to 0xB8000
    mov esi, protectedModeMsg
    call print_string32
    jmp hang32

%include "boot/bios/utils/print32.asm"