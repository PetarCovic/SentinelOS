#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>
#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/drivers/keyboard.hpp>
#include <sentinel/console.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();
    sentinel::logger::log_info("SentinelOS Kernel Loaded");
    sentinel::logger::log_info("Terminal Initialized");

    sentinel::arch::x86_64::idt::initialize();
    sentinel::logger::log_info("IDT Initialized.");

    sentinel::arch::x86_64::pic::remap();
    sentinel::logger::log_info("PIC Remapped");

    sentinel::drivers::keyboard::initialize();
    sentinel::logger::log_info("Keyboard Initialized");

    sentinel::logger::log_info("Console Initialized");
    sentinel::console::initialize();

    sentinel::logger::log_info("Enabling Interrupts");
    __asm__ volatile("sti");

    while(true)
    {
        sentinel::drivers::keyboard::KeyEvent event;
        
        if(sentinel::drivers::keyboard::read_event(event))
        {
            sentinel::console::handle_key_event(event);
        }

        __asm__ volatile ("hlt");
    }
}