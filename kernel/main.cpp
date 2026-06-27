#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>
#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/pic.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();

    sentinel::logger::log_info("SentinelOS Kernel Loaded");
    sentinel::logger::log_info("Terminal Initialized");

    sentinel::arch::x86_64::idt::initialize();

    sentinel::logger::log_info("IDT loaded.");

    sentinel::logger::log_info("Remapping PIC...");
    sentinel::arch::x86_64::pic::remap();
    sentinel::logger::log_info("PIC Remapped");

    while(true)
    {
        __asm__ volatile ("hlt");
    }
}