#pragma once

#include <sentinel/types.hpp>
#include <sentinel/boot/memory_map.hpp>

namespace sentinel::memory
{
    static constexpr sentinel::u64 MIN_ALLOCATABLE_ADDRESS=0x100000;
    static constexpr sentinel::u32 MAX_USABLE_REGIONS=32;
    static constexpr sentinel::u32 MAX_RESERVED_REGIONS=32;


    struct PhysicalMemoryRegion
    {
        sentinel::u64 start;
        sentinel::u64 end;
        sentinel::u64 length;
    };


    struct ReservedMemoryRegion
    {
        sentinel::u64 start;
        sentinel::u64 end;
        const char* name;
    };

    void initialize(const sentinel::boot::BootInfo* boot_info);

    sentinel::u32 get_usable_region_count();

    const PhysicalMemoryRegion* get_usable_regions();

    void print_usable_regions();

    sentinel::u64 get_kernel_start();

    sentinel::u64 get_kernel_end();

    sentinel::u64 get_kernel_size();

    void print_kernel_memory_layout();

    void reserve_region(sentinel::u64 start, sentinel::u64 end, const char*  name);

    sentinel::u32 get_reserved_region_count();

    const ReservedMemoryRegion* get_reserved_regions();

    void print_reserved_regions();
}