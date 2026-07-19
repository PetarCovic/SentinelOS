#include <sentinel/memory/physical_memory.hpp>
#include <sentinel/boot/memory_map.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/string.hpp>
#include <sentinel/memory/page.hpp>

extern "C" char __kernel_start;
extern "C" char __kernel_end;

namespace sentinel::memory
{
    static PhysicalMemoryRegion usable_regions[MAX_USABLE_REGIONS];
    static sentinel::u32 usable_region_count=0;

    static ReservedMemoryRegion reserved_regions[MAX_RESERVED_REGIONS];
    static sentinel::u32 reserved_region_count=0;

    void initialize(const sentinel::boot::BootInfo* boot_info)
    {
        usable_region_count=0;
        reserved_region_count=0;

        if(boot_info==nullptr)
        {
            return;
        }

        if(boot_info->memory_map==nullptr)
        {
            return;
        }

        sentinel::u32 entry_count=boot_info->memory_map_entry_count;

        for(sentinel::u32 i=0; i<entry_count; i++)
        {
            sentinel::boot::MemoryMapEntry entry=boot_info->memory_map[i];

            if(entry.type!=sentinel::boot::USABLE)
            {
                continue;
            }

            if(entry.length==0)
            {
                continue;
            }

            sentinel::u64 region_start=entry.base_addr;
            sentinel::u64 region_end=region_start+entry.length;

            if(region_end<=MIN_ALLOCATABLE_ADDRESS)
            {
                continue;
            }

            if(region_start<MIN_ALLOCATABLE_ADDRESS)
            {
                region_start=MIN_ALLOCATABLE_ADDRESS;
            }

            if(usable_region_count>=MAX_USABLE_REGIONS)
            {
                break;
            }

            region_start=sentinel::memory::align_page_up(region_start);
            region_end=sentinel::memory::align_page_down(region_end);

            if(region_start>=region_end)
            {
                continue;
            }

            sentinel::u64 region_length=region_end-region_start;

            usable_regions[usable_region_count].start=region_start;
            usable_regions[usable_region_count].end=region_end;
            usable_regions[usable_region_count].length=region_length;
            usable_region_count++;
        }

        reserve_region(get_kernel_start(), get_kernel_end(), "Kernel");

        reserve_region(0x0000, 0x100000, "Low Memory / BIOS / Boot Area");

        reserve_region(0x1000, 0x2000, "PML4");
        reserve_region(0x2000, 0x3000, "PDPT");
        reserve_region(0x3000, 0x4000, "Page Directory");

        reserve_region(0x5000, 0x6000, "E820 Memory Map");
        reserve_region(0x7000, 0x8000, "BootInfo");

        reserve_region(0x80000, 0x90000, "Kernel Stack");

        reserve_region(0xB8000, 0xC0000, "VGA Text Buffer");
    }

    sentinel::u32 get_usable_region_count()
    {
        return usable_region_count;
    }

    const PhysicalMemoryRegion* get_usable_regions()
    {
        return usable_regions;
    }

    void print_usable_regions()
    {
        if(usable_region_count==0)
        {
            sentinel::terminal::writeln("No Usable Regions Found");
            return;
        }

        sentinel::terminal::writeln("Filtered Usable Memory Regions");

        for(sentinel::u32 i=0; i<usable_region_count; i++)
        {
            sentinel::terminal::write("Region ");
            sentinel::terminal::writeln_u64(i);

            sentinel::terminal::write("Start:  ");
            sentinel::terminal::writeln_hex(usable_regions[i].start);
            
            sentinel::terminal::write("End:    ");
            sentinel::terminal::writeln_hex(usable_regions[i].end);

            sentinel::terminal::write("Length: ");
            sentinel::terminal::writeln_hex(usable_regions[i].length);
        }
    }

    sentinel::u64 get_kernel_start()
    {
        return reinterpret_cast<sentinel::u64>(&__kernel_start);
    }

    sentinel::u64 get_kernel_end()
    {
        return reinterpret_cast<sentinel::u64>(&__kernel_end);
    }

    sentinel::u64 get_kernel_size()
    {
        return get_kernel_end()-get_kernel_start();
    }

    void print_kernel_memory_layout()
    {
        sentinel::terminal::writeln("Kernel Memory Layout");

        sentinel::terminal::write("Start: ");
        sentinel::terminal::writeln_hex(get_kernel_start());

        sentinel::terminal::write("End:   ");
        sentinel::terminal::writeln_hex(get_kernel_end());

        sentinel::terminal::write("Size:  ");
        sentinel::terminal::writeln_hex(get_kernel_size());
    }

    void reserve_region(sentinel::u64 start, sentinel::u64 end, const char*  name)
    {
        start=sentinel::memory::align_page_down(start);
        end=sentinel::memory::align_page_up(end);

        if(start>=end)
        {
            return;
        }

        if(reserved_region_count>=MAX_RESERVED_REGIONS)
        {
            return;
        }

        if(sentinel::string::is_empty(name))
        {
            name="RESERVED";
        }

        reserved_regions[reserved_region_count].start=start;
        reserved_regions[reserved_region_count].end=end;
        reserved_regions[reserved_region_count].name=name;

        reserved_region_count++;
    }

    sentinel::u32 get_reserved_region_count()
    {
        return reserved_region_count;
    }

    const ReservedMemoryRegion* get_reserved_regions()
    {
        return reserved_regions;
    }

    void print_reserved_regions()
    {
        if(reserved_region_count==0)
        {
            sentinel::terminal::writeln("No Reserved Regions Found");
            return;
        }

        sentinel::terminal::writeln("Reserved Memory Regions");

        for(sentinel::u32 i=0; i<reserved_region_count; i++)
        {
            sentinel::terminal::write("Start: ");
            sentinel::terminal::writeln_hex(reserved_regions[i].start);

            sentinel::terminal::write("End:   ");
            sentinel::terminal::writeln_hex(reserved_regions[i].end);

            sentinel::terminal::write("Name:  ");
            sentinel::terminal::writeln(reserved_regions[i].name);
        }
    }
}