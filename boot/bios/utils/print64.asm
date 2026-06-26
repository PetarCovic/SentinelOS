; print64.asm
; 64-bit long mode VGA string printing utility
;
; Input:
;   RSI = null-terminated string
;
; Destroys:
;   RAX, RDI, RSI

print_string64:
    mov rdi, 0xB8000 + 160 * 4   ; row 4, column 0
    mov ah, 0x0F                 ; white text on black background

.next_char:
    lodsb
    cmp al, 0
    je .done

    mov [rdi], ax                ; write character + color attribute
    add rdi, 2                   ; next VGA cell
    jmp .next_char

.done:
    ret