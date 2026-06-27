#pragma once

#include <sentinel/types.hpp>

namespace sentinel::arch::x86_64::pic
{
    void remap();

    void send_eoi(sentinel::u8 irq);

    void mask_irq(sentinel::u8 irq);

    void unmask_irq(sentinel::u8 irq);

    void mask_all();
}