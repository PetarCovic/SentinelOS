[BITS 64]

global default_interrupt_stub
global isr0_stub
global isr6_stub
global isr13_stub
global isr14_stub

extern default_interrupt_handler
extern isr0_handler
extern isr6_handler
extern isr13_handler
extern isr14_handler

default_interrupt_stub:
    cli
    call default_interrupt_handler

.default_hang:
    hlt
    jmp .default_hang

isr0_stub:
    cli
    call isr0_handler

.isr0_hang:
    hlt
    jmp .isr0_hang

isr6_stub:
    cli
    call isr6_handler

.isr6_hang:
    hlt
    jmp .isr6_hang

isr13_stub:
    cli
    call isr13_handler

.isr13_hang:
    hlt
    jmp .isr13_hang

isr14_stub:
    cli
    call isr14_handler

.isr14_hang:
    hlt
    jmp .isr14_hang