#include <sentinel/terminal.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();

    sentinel::terminal::writeln("SentinelOS Kernel Loaded");
    sentinel::terminal::writeln("Terminal Initialized");

    while(true)
    {
        __asm__ volatile ("hlt");
    }
}