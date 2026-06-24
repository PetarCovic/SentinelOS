[BITS 16]
[ORG 0x7C00]

_start:
    ;Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;Set up stack
    mov ss, ax
    mov sp, 0x7C00

    ;Print boot BootMessage
    mov si, bootMsg
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

bootMsg:
    db "SentinelOS Booting...", 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55