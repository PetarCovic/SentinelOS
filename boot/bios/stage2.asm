[BITS 16]
[ORG 0x8000]

stage2_start:
    ;Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;Setup stack
    cli             ;Clear interrupt flag
    mov ss, ax
    mov sp, 0x7C00
    sti             ;Set interrupt flag

    ;Make sure string instructions move forward
    cld             ;Clear direction flag

    ;Print stage2 message
    mov si, stage2Msg
    call print_string

hang:
    hlt
    jmp hang

print_string:
    mov ah, 0x0E

.next_char:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .next_char

.done:
    ret

stage2Msg:
    db "Stage 2 Loaded", 13, 10, 0