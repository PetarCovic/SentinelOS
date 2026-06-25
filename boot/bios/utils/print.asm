;print.asm
;16-bit BIOS string printing utility
;
;Input:
;   DS:SI=address of null-terminated string
;
;Destroys:
;   AX, SI

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