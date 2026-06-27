#include <sentinel/arch/x86_64/interrupts.hpp>
#include <sentinel/panic.hpp>

extern "C" void default_interrupt_handler()
{
    sentinel::panic("Unhandled interrupt.");
}

extern "C" void isr0_handler()
{
    sentinel::panic("CPU Exception: Divide-by-zero.");
}

extern "C" void isr6_handler()
{
    sentinel::panic("CPU Exception: Invalid opcode.");
}

extern "C" void isr13_handler()
{
    sentinel::panic("CPU Exception: General protection fault.");
}

extern "C" void isr14_handler()
{
    sentinel::panic("CPU Exception: Page fault.");
}