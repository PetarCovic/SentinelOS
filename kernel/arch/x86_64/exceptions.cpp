#include <sentinel/arch/x86_64/interrupt_frame.hpp>
#include <sentinel/panic.hpp>

extern "C" void exception_dispatcher(sentinel::arch::x86_64::InterruptFrame* frame)
{
    switch (frame->vector)
    {
        case 0:
            sentinel::panic("CPU Exception: Divide-by-zero.");
            break;

        case 6:
            sentinel::panic("CPU Exception: Invalid opcode.");
            break;

        case 13:
            sentinel::panic("CPU Exception: General protection fault.");
            break;

        case 14:
            sentinel::panic("CPU Exception: Page fault.");
            break;

        default:
            sentinel::panic("CPU Exception: Unknown exception.");
            break;
    }
}