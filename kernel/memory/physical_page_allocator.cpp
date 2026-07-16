#include <sentinel/memory/physical_page_allocator.hpp>
#include <sentinel/types.hpp>
#include <sentinel/memory/physical_memory.hpp>
#include <sentinel/memory/page.hpp>
#include <sentinel/terminal.hpp>

namespace sentinel::memory::physical_page_allocator
{
    static void mark_all_regions_used();
    static void mark_region_free(sentinel::u64 start, sentinel::u64 end);
    static void mark_region_used(sentinel::u64 start, sentinel::u64 end);

    static sentinel::u8 page_bitmap[BITMAP_SIZE_BYTES];
    
    void initialize()
    {
        mark_all_regions_used();

        const PhysicalMemoryRegion* usable_regions
            =sentinel::memory::get_usable_regions();

        for(sentinel::u64 i=0; i<sentinel::memory::get_usable_region_count(); i++)
        {
            mark_region_free(usable_regions[i].start, usable_regions[i].end);
        }

        const ReservedMemoryRegion* reserved_regions
            =sentinel::memory::get_reserved_regions();

        for(sentinel::u64 i=0; i<sentinel::memory::get_reserved_region_count(); i++)
        {
            mark_region_used(reserved_regions[i].start, reserved_regions[i].end);
        }
    }

    void print_stats()
    {
        sentinel::terminal::write("Total pages tracked: ");
        sentinel::terminal::writeln_u64(TOTAL_PAGES);

        sentinel::u64 free_count=get_free_page_count();
        sentinel::u64 used_count=get_used_page_count();

        sentinel::u64 free_mib=(free_count*PAGE_SIZE)/1048576;
        sentinel::u64 used_mib=(used_count*PAGE_SIZE)/1048576;

        sentinel::terminal::write("Free Pages: ");
        sentinel::terminal::writeln_u64(free_count);

        sentinel::terminal::write("Used Pages: ");
        sentinel::terminal::writeln_u64(used_count);

        sentinel::terminal::write("Free Memory in MiB: ");
        sentinel::terminal::writeln_u64(free_mib);

        sentinel::terminal::write("Used Memory in MiB: ");
        sentinel::terminal::writeln_u64(used_mib);
    }

    void mark_page_used(sentinel::u64 page_address)
    {
        sentinel::u64 page_index=page_address/PAGE_SIZE;

        if(page_index>=TOTAL_PAGES)
        {
            return;
        }

        sentinel::u64 byte_index=page_index/8;
        sentinel::u64 bit_index=page_index%8;

        sentinel::u8 mask=static_cast<sentinel::u8>(1 << bit_index);

        page_bitmap[byte_index] |= mask;
    }

    void mark_page_free(sentinel::u64 page_address)
    {
        sentinel::u64 page_index=page_address/PAGE_SIZE;

        if(page_index>=TOTAL_PAGES)
        {
            return;
        }

        sentinel::u64 byte_index=page_index/8;
        sentinel::u64 bit_index=page_index%8;

        sentinel::u8 mask=static_cast<sentinel::u8>(1 << bit_index);

        page_bitmap[byte_index] &= static_cast<sentinel::u8>(~mask);
    }

    bool is_page_used(sentinel::u64 page_address)
    {

        sentinel::u64 page_index=page_address/PAGE_SIZE;
        
        if(page_index>=TOTAL_PAGES)
        {
            return true;
        }

        sentinel::u64 byte_index=page_index/8;
        sentinel::u64 bit_index=page_index%8;

        sentinel::u8 mask=static_cast<sentinel::u8>(1 << bit_index);

        return (page_bitmap[byte_index] & mask)!=0;
    }

    bool is_page_free(sentinel::u64 page_address)
    {
        return !is_page_used(page_address);
    }

    sentinel::u64 allocate_page()
    {
        for(sentinel::u64 i=0; i<TOTAL_PAGES; i++)
        {
            if(is_page_free(i*PAGE_SIZE))
            {
                mark_page_used(i*PAGE_SIZE);

                return i*PAGE_SIZE;
            }
        }

        return 0;
    }

    void free_page(sentinel::u64 page_address)
    {
        if(!owns_page(page_address))
        {
            return;
        }

        if(is_reserved_page(page_address))
        {
            return;
        }

        mark_page_free(page_address);
    }

    sentinel::u64 get_free_page_count()
    {
        sentinel::u64 free_count=0;

        for(sentinel::u64 i=0; i<TOTAL_PAGES; i++)
        {
            if(!is_page_used(PAGE_SIZE*i))
            {
                free_count++;
            }
        }

        return free_count;
    }

    sentinel::u64 get_used_page_count()
    {
        sentinel::u64 used_count=0;

        for(sentinel::u64 i=0; i<TOTAL_PAGES; i++)
        {
            if(is_page_used(PAGE_SIZE*i))
            {
                used_count++;
            }
        }

        return used_count;
    }

    sentinel::u64 get_total_page_count()
    {
        return TOTAL_PAGES;
    }

    bool owns_page(sentinel::u64 page_address)
    {
        return sentinel::memory::is_page_aligned(page_address) 
                && page_address<MAX_PHYSICAL_MEMORY;
    }

    bool is_reserved_page(sentinel::u64 page_address)
    {
        if(!owns_page(page_address))
        {
            return false;
        }

        const ReservedMemoryRegion* reserved_regions
            =sentinel::memory::get_reserved_regions();

        for(sentinel::u64 i=0; i<sentinel::memory::get_reserved_region_count(); i++)
        {
            if(page_address>=reserved_regions[i].start 
                && page_address<reserved_regions[i].end)
            {
                return true;
            }
        }

        return false;
    }

    static void mark_all_regions_used()
    {
        for(sentinel::u64 i=0; i<BITMAP_SIZE_BYTES; i++)
        {
            page_bitmap[i]=0xFF;
        }
    }

    static void mark_region_free(sentinel::u64 start, sentinel::u64 end)
    {
        for(sentinel::u64 address=start; address<end; address+=PAGE_SIZE)
        {
            mark_page_free(address);
        }
    }

    static void mark_region_used(sentinel::u64 start, sentinel::u64 end)
    {
        for(sentinel::u64 address=start; address<end; address+=PAGE_SIZE)
        {
            mark_page_used(address);
        }
    }
}