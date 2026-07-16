#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>
#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/drivers/keyboard.hpp>
#include <sentinel/console.hpp>
#include <sentinel/shell.hpp>
#include <sentinel/boot/memory_map.hpp>
#include <sentinel/memory/physical_memory.hpp>

extern "C" void kernel_main(const sentinel::boot::BootInfo* boot_info)
{
    sentinel::terminal::initialize();
    sentinel::logger::log_info("SentinelOS Kernel Loaded");
    sentinel::logger::log_info("Terminal Initialized");
    sentinel::boot::print_memory_map(boot_info);

    sentinel::arch::x86_64::idt::initialize();
    sentinel::logger::log_info("IDT Initialized.");

    sentinel::arch::x86_64::pic::remap();
    sentinel::logger::log_info("PIC Remapped");

    sentinel::drivers::keyboard::initialize();
    sentinel::logger::log_info("Keyboard Initialized");

    sentinel::logger::log_info("Initializing Console");
    sentinel::logger::log_info("Enabling Interrupts");
    sentinel::console::initialize();
    __asm__ volatile("sti");

    while(true)
    {
        sentinel::drivers::keyboard::KeyEvent event;
        
        while(sentinel::drivers::keyboard::read_event(event))
        {
            sentinel::console::handle_key_event(event);
        }

        char command[sentinel::console::get_command_buffer_size()];

        if(sentinel::console::read_line(command))
        {
            sentinel::shell::execute_command(command);
            sentinel::console::print_prompt();
        }

        sentinel::memory::initialize(boot_info);
        sentinel::terminal::writeln("Memory Initialized");

        //sentinel::boot::print_memory_map(boot_info);
        ///sentinel::memory::print_kernel_memory_layout();
        //sentinel::memory::print_usable_regions();
        //sentinel::memory::print_reserved_regions();

        sentinel::console::print_prompt();

        __asm__ volatile ("hlt");
    }
}