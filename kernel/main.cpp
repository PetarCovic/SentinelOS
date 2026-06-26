#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>

extern "C" void kernel_main()
{
    sentinel::terminal::initialize();

    sentinel::terminal::writeln("SentinelOS Kernel Loaded");
    sentinel::terminal::writeln("Terminal Initialized");

    sentinel::logger::log_info("INFO TEST");
    sentinel::logger::log_warn("WARN TEST");
    sentinel::logger::log_error("ERROR TEST");


    while(true)
    {
        __asm__ volatile ("hlt");
    }
}