#pragma once

#include <sentinel/types.hpp>

namespace sentinel::arch::x86_64::io
{
    sentinel::u8 inb(sentinel::u16 port);

    void outb(sentinel::u16 port, sentinel::u8 value);

    void io_wait();
}