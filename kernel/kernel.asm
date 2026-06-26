[BITS 16]

global kernel_start
extern kernel_main


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
    jmp CODE32_SEG:init_pm

hang16:
    hlt
    jmp hang16

kernelMsg:
    db "Kernel Loaded", 13, 10, 0

%include "boot/bios/utils/print16.asm"

gdt_start:
    ; Null descriptor
    dq 0

gdt_code32:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

gdt_code64:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 10101111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE32_SEG equ gdt_code32-gdt_start
CODE64_SEG equ gdt_code64-gdt_start
DATA_SEG equ gdt_data-gdt_start

PML4_BASE equ 0x1000
PDPT_BASE equ 0x2000
PD_BASE equ 0x3000

PAGE_PRESENT equ 1
PAGE_WRITE equ 2
PAGE_SIZE equ 1<<7

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

    ;Print protected mode reached to 0xB8000
    mov esi, protectedModeMsg
    call print_string32

    call init_lm

hang32:
    hlt
    jmp hang32

verify_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb no_long_mode

    mov eax, 0x80000001
    cpuid
    test edx, 1<<29
    jz no_long_mode

    ret

no_long_mode:
    mov esi, noLongModeMsg
    call print_string32
    jmp hang32

protectedModeMsg:
    db "Protected Mode Reached", 0

noLongModeMsg:
    db "CPU does not support long mode", 0

%include "boot/bios/utils/print32.asm"

init_lm:
    call verify_long_mode
    call setup_page_tables
    call enable_pae
    call load_pml4
    call enable_long_mode
    call enable_paging

    jmp CODE64_SEG:long_mode_entry

enable_pae:
    mov eax, cr4
    or eax, 1<<5        ;PAE
    mov cr4, eax
    ret

load_pml4:
    mov eax, PML4_BASE
    mov cr3, eax
    ret

enable_long_mode:
    mov ecx, 0xC0000080     ;EFER MSR
    rdmsr
    or eax, 1<<8            ;EFER.LME=1
    wrmsr
    ret

enable_paging:
    mov eax, cr0
    or  eax, (1<<31)        ;CR0.PG=1
    mov cr0, eax
    ret

setup_page_tables:
    ;Clear PML4, PDPT, and PD setup_page_tables
    mov edi, PML4_BASE
    mov ecx, 4096*3/4
    xor eax, eax
    rep stosd

    ;PML4[0]->PDPT
    mov dword [PML4_BASE], PDPT_BASE | PAGE_PRESENT | PAGE_WRITE
    mov dword [PML4_BASE+4], 0

    ;PDPT[0]->Page  Directory
    mov dword [PDPT_BASE], PD_BASE | PAGE_PRESENT | PAGE_WRITE
    mov dword [PDPT_BASE+4], 0

    ;PD[0]->2 MiB page starting at physical 0x00000000
    mov dword [PD_BASE], PAGE_PRESENT | PAGE_WRITE | PAGE_SIZE
    mov dword [PD_BASE+4], 0

    ret

longModeMsg:
    db "Long Mode Reached", 0

[BITS 64]

long_mode_entry:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, 0x90000
    cld

    mov rsi, longModeMsg
    call print_string64

    call kernel_main

hang64:
    hlt
    jmp hang64
    
%include "boot/bios/utils/print64.asm"