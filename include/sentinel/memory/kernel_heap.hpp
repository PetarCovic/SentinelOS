#pragma once

#include <sentinel/types.hpp>

namespace sentinel::memory::kernel_heap
{
    void initialize();

    bool is_initialized();

    sentinel::u64 get_heap_start();

    sentinel::u64 get_heap_end();

    sentinel::u64 get_heap_size();

    void print_heap_info();
}