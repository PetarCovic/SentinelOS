#pragma once

#include <sentinel/types.hpp>

namespace sentinel::arch::x86_64::cpu
{
    sentinel::u64 read_rflags();

    void disable_interrupts();

    void enable_interrupts();

    void restore_rflags(sentinel::u64 flags);

    bool interrupts_enabled();
}