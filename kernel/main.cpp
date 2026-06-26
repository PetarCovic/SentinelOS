#include <sentinel/terminal.hpp>
#include<sentinel/panic.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();

    sentinel::terminal::writeln("SentinelOS Kernel Loaded");
    sentinel::terminal::writeln("Terminal Initialized");

    sentinel::panic("Testing panic handler.");

    while(true)
    {
        __asm__ volatile ("hlt");
    }
}