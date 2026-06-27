#pragma once

#include <sentinel/types.hpp>

namespace sentinel::arch::x86_64
{
    struct InterruptFrame
    {
        sentinel::u64 rax;
        sentinel::u64 rbx;
        sentinel::u64 rcx;
        sentinel::u64 rdx;
        sentinel::u64 rbp;
        sentinel::u64 rdi;
        sentinel::u64 rsi;
        sentinel::u64 r8;
        sentinel::u64 r9;
        sentinel::u64 r10;
        sentinel::u64 r11;
        sentinel::u64 r12;
        sentinel::u64 r13;
        sentinel::u64 r14;
        sentinel::u64 r15;

        sentinel::u64 vector;
        sentinel::u64 error_code;

        sentinel::u64 rip;
        sentinel::u64 cs;
        sentinel::u64 rflags;
    };
}