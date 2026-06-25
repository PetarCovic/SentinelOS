[BITS 16]
[ORG 0x7C00]

STAGE2_LOAD_ADDR equ 0x8000
STAGE2_SECTORS equ 1

_start:
    ;Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;Save boot drive passed by BIOS in DL
    mov [bootDrive], dl

    ;Set up stack
    cli
    mov ss, ax
    mov sp, 0x7C00
    sti

    ;Make sure string instructions move forward
    cld

    ;Print boot BootMessage
    mov si, bootMsg
    call print_string

    ;Load stage2 from disk
    call load_stage2

    ;Jump to stage 2 at 0x0000:0x8000
    jmp 0x0000:STAGE2_LOAD_ADDR

hang:
    hlt
    jmp hang

load_stage2:
    ;BIOS int 0x13, function 0x02=read sectors
    mov ah, 0x02                ;read sectors
    mov al, STAGE2_SECTORS      ;number of sectors to read

    mov ch, 0x00                ;cylinder 0
    mov cl, 0x02                ;sector 2, because sector 1 is bootsector
    mov dh, 0x00                ;head 0
    mov dl, [bootDrive]         ;boot drive from BIOS
    
    xor ax, ax
    mov es, ax
    mov bx, STAGE2_LOAD_ADDR    ;ES:BX=0000:8000
    int 0x13

    jc disk_error               ;carry flag set=read failed
    ret

disk_error:
    mov si, diskErrorMsg
    call print_string
    jmp hang

print_string:
    mov ah, 0x0E

.next_char:
    lodsb           ;Loads byte into address
    cmp al, 0
    je .done
    int 0x10
    jmp .next_char

.done:
    ret

bootDrive:
    db 0

bootMsg:
    db "SentinelOS Booting...", 13, 10, 0

diskErrorMsg:
    db "Disk read error!", 13, 10, 0

times 510-($-$$) db 0
dw 0xAA55