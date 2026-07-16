#pragma once

#include <sentinel/types.hpp>

namespace sentinel::memory
{
    static constexpr sentinel::u64 PAGE_SIZE=4096;

    sentinel::u64 align_down(sentinel::u64 address, sentinel::u64 alignment);
    sentinel::u64 align_up(sentinel::u64 address, sentinel::u64 alignment);

    sentinel::u64 align_page_down(sentinel::u64 address);
    sentinel::u64 align_page_up(sentinel::u64 address);

    bool is_aligned(sentinel::u64 address, sentinel::u64 alignment);
    bool is_page_aligned(sentinel::u64 address);
} 
