#pragma once

#include <sentinel/types.hpp>
#include <sentinel/memory/page.hpp>

namespace sentinel::memory::physical_page_allocator
{
    static constexpr sentinel::u64 MAX_PHYSICAL_MEMORY=536870912;
    static constexpr sentinel::u64 TOTAL_PAGES=MAX_PHYSICAL_MEMORY/PAGE_SIZE;
    static constexpr sentinel::u64 BITMAP_SIZE_BYTES=TOTAL_PAGES/8;

    struct PageMetadata
    {
        bool allocated;
        bool reserved;
    };

    void initialize();

    void print_stats();

    void mark_page_used(sentinel::u64 page_address);
    void mark_page_free(sentinel::u64 page_address);

    bool is_page_used(sentinel::u64 page_address);
    bool is_page_free(sentinel::u64 page_address);

    sentinel::u64 allocate_page();

    sentinel::u64 allocate_contiguous_pages(sentinel::u64 page_count);

    void free_page(sentinel::u64 page_address);

    void  free_contiguous_pages(sentinel::u64 start_address, sentinel::u64 page_count);

    sentinel::u64 get_free_page_count();

    sentinel::u64 get_used_page_count();

    sentinel::u64 get_total_page_count();

    bool owns_page(sentinel::u64 page_address);

    bool is_reserved_page(sentinel::u64 page_address);

    bool is_metadata_allocated(sentinel::u64 page_address);

    bool is_metadata_reserved(sentinel::u64 page_address);
}