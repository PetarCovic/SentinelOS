#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>
#include <sentinel/arch/x86_64/idt.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();

    sentinel::logger::log_info("SentinelOS Kernel Loaded");
    sentinel::logger::log_info("Terminal Initialized");

    sentinel::arch::x86_64::idt::initialize();

    sentinel::logger::log_info("IDT loaded.");

    sentinel::logger::log_info("About to trigger raw UD2.");
__asm__ volatile (".byte 0x0F, 0x0B");
sentinel::logger::log_info("This should never print.");

    while(true)
    {
        __asm__ volatile ("hlt");
    }
}