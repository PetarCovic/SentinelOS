#pragma once

namespace sentinel::arch::x86_64::idt
{
    void initialize();

    void set_entry(int index, void (*handler)(), unsigned char ist, unsigned char type_attr);

    void load();
}