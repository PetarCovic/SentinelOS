[BITS 16]
[ORG 0x8000]

KERNEL_LOAD_ADDR equ 0x9000
KERNEL_SECTORS equ 64

stage2_start:
    ;Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;Save boot drive passed from bootsector in DL
    mov [bootDrive], dl

    ;Setup stack
    cli             ;Clear interrupt flag
    mov ss, ax
    mov sp, 0x7C00
    sti             ;Set interrupt flag

    ;Make sure string instructions move forward
    cld             ;Clear direction flag

    ;Print stage2 message
    mov si, stage2Msg
    call print_string16

    ;Load kernel from disk
    call load_kernel

    ;Jump to kernel from disk
    jmp 0x0000:KERNEL_LOAD_ADDR

hang:
    hlt
    jmp hang

load_kernel:
    ;Set ES:BX= Destination address
    xor ax, ax
    mov es, ax
    mov bx, KERNEL_LOAD_ADDR

    ;BIOS int 0x13, function 0x02=read sectors
    mov ah,  0x02
    mov al, KERNEL_SECTORS

    mov ch, 0x00
    mov cl, 0x03
    mov dh, 0x00
    mov dl, [bootDrive]

    int 0x13

    jc  disk_error
    ret

disk_error:
    mov si, diskErrorMsg
    call print_string16
    jmp hang

bootDrive:
    db 0

diskErrorMsg:
    db "Disk read error!", 13, 10, 0

stage2Msg:
    db "Stage 2 Loaded", 13, 10, 0

%include  "boot/bios/utils/print16.asm"