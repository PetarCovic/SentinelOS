;print32.asm
;32-bit VGA string printing utility
;
;Input:
;   ESI=null-terminated string
;
;Destroys:
;   EAX, EBX, EDI, ESI

print_string32:
    mov edi, 0xB8000+160*3   ;row 3, column 0
    mov ah, 0x0F            ;White text on black background

.next_char:
    lodsb
    cmp al, 0
    je .done
    
    mov [edi], AX           ;Write character + color attribute
    add edi, 2              ;Next VGA cell
    jmp .next_char

.done:
    ret