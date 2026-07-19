#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>
#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/drivers/keyboard.hpp>
#include <sentinel/console.hpp>
#include <sentinel/shell.hpp>
#include <sentinel/boot/memory_map.hpp>
#include <sentinel/memory/physical_memory.hpp>
#include <sentinel/memory/physical_page_allocator.hpp>
#include <sentinel/memory/kernel_heap.hpp>

extern "C" void kernel_main(const sentinel::boot::BootInfo* boot_info)
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

    sentinel::console::initialize();
    sentinel::logger::log_info("Console Initialized");

    sentinel::memory::initialize(boot_info);
    sentinel::memory::physical_page_allocator::initialize();
    sentinel::logger::log_info("Memory Initialized");

    sentinel::memory::kernel_heap::initialize();
    sentinel::logger::log_info("Kernel Heap Initialized");

    sentinel::logger::log_info("Enabling Interrupts");
    __asm__ volatile("sti");
    sentinel::console::print_prompt();

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

        __asm__ volatile ("hlt");
    }
}