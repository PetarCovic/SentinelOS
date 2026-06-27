#include <sentinel/arch/x86_64/io.hpp>

namespace sentinel::arch::x86_64::io
{
    sentinel::u8 inb(sentinel::u16 port)
    {
        sentinel::u8 value;

        __asm__ volatile("inb %1, %0" : "=a"(value) : "Nd"(port));

        return value;
    }

    void outb(sentinel::u16 port, sentinel::u8 value)
    {
        __asm__ volatile("outb %0, %1" :  : "a"(value), "Nd"(port));
    }

    void io_wait()
    {
        outb(0x80, 0);
    }
}