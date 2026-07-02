#pragma once

#include <sentinel/types.hpp>

namespace sentinel::boot
{
    enum MemoryType : sentinel::u32
    {
        USABLE              = 1,
        RESERVED            = 2,
        ACPI_RECLAIMABLE    = 3,
        ACPI_NVS            = 4,
        BAD_MEMORY          = 5,
    };

    static constexpr sentinel::u32 E820_TYPE_USABLE = 1;
    static constexpr sentinel::u32 E820_TYPE_RESERVED = 2;
    static constexpr sentinel::u32 E820_TYPE_ACPI_RECLAIMABLE = 3;
    static constexpr sentinel::u32 E820_TYPE_ACPI_NVS = 4;
    static constexpr sentinel::u32 E820_TYPE_BAD_MEMORY = 5;

    static constexpr sentinel::u32 E820_ATTR_VALID = 1 << 0;
    static constexpr sentinel::u32 E820_ATTR_NON_VOLATILE = 1 << 1;

    struct MemoryMapEntry
    {
        sentinel::u64 base_addr;
        sentinel::u64 length;
        MemoryType type;
        sentinel::u32 attributes;
    } __attribute__ ((packed));

    struct BootInfo
    {
        MemoryMapEntry* memory_map;
        sentinel::u32 memory_map_entry_count;
    } __attribute__ ((packed));

    void print_memory_map(const BootInfo* boot_info);

    const char* memory_type_to_string(MemoryType type);
}