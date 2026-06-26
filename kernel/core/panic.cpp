#include <sentinel/terminal.hpp>
#include<sentinel/panic.hpp>

namespace sentinel
{
    [[noreturn]] void panic(const char* msg)
    {
        sentinel::terminal::clear();
        sentinel::terminal::set_color(0x04);
        sentinel::terminal::writeln("KERNEL PANIC");
        terminal::writeln(msg != nullptr ? msg : "No panic message provided.");

        while(true)
        {
            __asm__ volatile ("cli");
            __asm__ volatile("hlt");
        }
    }
}