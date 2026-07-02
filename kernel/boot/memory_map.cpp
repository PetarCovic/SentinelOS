#include <sentinel/boot/memory_map.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/types.hpp>

namespace sentinel::boot
{
    void print_memory_map(const BootInfo* boot_info)
    {
        if(boot_info==nullptr)
        {
            return;
        }

        if(boot_info->memory_map==nullptr)
        {
            return;
        }

        sentinel::terminal::write("Memory Map Entry Count: ");
        sentinel::u32 entry_count=boot_info->memory_map_entry_count;
        sentinel::terminal::writeln_u64(entry_count);

        for(int i=0; i<entry_count; i++)
        {
            MemoryMapEntry memory_map=boot_info->memory_map[i];

            sentinel::terminal::write("Base: ");
            sentinel::terminal::writeln_hex(memory_map.base_addr);

            sentinel::terminal::write("Length: ");
            sentinel::terminal::writeln_hex(memory_map.length);

            sentinel::terminal::write("Type: ");
            sentinel::terminal::write_u64(memory_map.type);
            sentinel::terminal::write(" (");
            sentinel::terminal::write(memory_type_to_string(memory_map.type));
            sentinel::terminal::writeln(")");

            sentinel::terminal::write("Attributes: ");
            sentinel::terminal::writeln_u64(memory_map.attributes);
        }
    }

    const char* memory_type_to_string(MemoryType type)
    {
        switch(type)
        {
            case MemoryType::USABLE:
                return "Usable";

            case MemoryType::RESERVED:
                return "Reserved";
            
            case MemoryType::ACPI_RECLAIMABLE:
                return "ACPI Reclaimable";

            case MemoryType::ACPI_NVS:
                return "ACPI NVS";

            case MemoryType::BAD_MEMORY:
                return "Bad Memory";

            default:
                return "Unknown";
        }
    }
}