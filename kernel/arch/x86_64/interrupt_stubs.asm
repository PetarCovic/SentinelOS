global default_interrupt_stub
extern default_interrupt_handler

default_interrupt_stub:
    cli
    call default_interrupt_handler

.hang:
    hlt
    jmp .hang