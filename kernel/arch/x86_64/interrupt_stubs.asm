[BITS 64]

global default_interrupt_stub
global isr0_stub
global isr6_stub
global isr13_stub
global isr14_stub

extern exception_dispatcher

; ------------------------------------------------------------
; Default interrupt stub
;
; Used for any interrupt vector that does not have a named
; handler yet.
;
; This follows the same normalized format:
;   push fake error code
;   push vector number
;   jump to common handler
; ------------------------------------------------------------

default_interrupt_stub:
    push qword 0        ; fake error code
    push qword 255      ; default/unknown vector
    jmp isr_common_stub


; ------------------------------------------------------------
; CPU exception stubs without CPU-pushed error codes
;
; Vector 0: Divide-by-zero
; Vector 6: Invalid opcode
;
; These exceptions do NOT automatically push an error code,
; so we push a fake one ourselves.
; ------------------------------------------------------------

isr0_stub:
    push qword 0        ; fake error code
    push qword 0        ; vector 0
    jmp isr_common_stub

isr6_stub:
    push qword 0        ; fake error code
    push qword 6        ; vector 6
    jmp isr_common_stub


; ------------------------------------------------------------
; CPU exception stubs with CPU-pushed error codes
;
; Vector 13: General protection fault
; Vector 14: Page fault
;
; These exceptions already have an error code pushed by the CPU.
; So we only push the vector number.
; ------------------------------------------------------------

isr13_stub:
    push qword 13       ; vector 13
    jmp isr_common_stub

isr14_stub:
    push qword 14       ; vector 14
    jmp isr_common_stub


; ------------------------------------------------------------
; Common ISR stub
;
; Stack layout after this saves registers:
;
;   rsp + 0    rax
;   rsp + 8    rbx
;   rsp + 16   rcx
;   rsp + 24   rdx
;   rsp + 32   rbp
;   rsp + 40   rdi
;   rsp + 48   rsi
;   rsp + 56   r8
;   rsp + 64   r9
;   rsp + 72   r10
;   rsp + 80   r11
;   rsp + 88   r12
;   rsp + 96   r13
;   rsp + 104  r14
;   rsp + 112  r15
;   rsp + 120  vector
;   rsp + 128  error_code
;   rsp + 136  rip
;   rsp + 144  cs
;   rsp + 152  rflags
;
; We pass rsp to C++ as:
;
;   exception_dispatcher(InterruptFrame* frame)
;
; For now, this is fatal and does not return with iretq.
; ------------------------------------------------------------

isr_common_stub:
    cli

    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rsi
    push rdi
    push rbp
    push rdx
    push rcx
    push rbx
    push rax

    mov rdi, rsp
    call exception_dispatcher

.hang:
    hlt
    jmp .hang