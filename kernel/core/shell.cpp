#include <sentinel/shell.hpp>
#include <sentinel/console.hpp>
#include <sentinel/string.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/panic.hpp>
#include <sentinel/memory/physical_page_allocator.hpp>
#include <sentinel/memory/physical_memory.hpp>
#include <sentinel/memory/kernel_heap.hpp>

namespace sentinel::shell
{
    static void execute_help();
    static void execute_clear();
    static void execute_version();
    static void execute_echo(const char* command);
    static void execute_panic_test();
    static void execute_idt_test();
    static void command_not_found();
    static void execute_memstats();
    static void execute_alloc_page();
    static void execute_free_page();
    static void execute_memmap();
    static void execute_reserved();
    static void execute_page_info(sentinel::u64 address);
    static void execute_alloc_contig(sentinel::u64 count);
    static void execute_free_contig();
    static void execute_heap_info();

    static sentinel::u64 last_test_page=0;
    static sentinel::u64 last_contiguous_page=0;
    static sentinel::u64 last_contiguous_count=0;

    void execute_command(const char* command)
    {
        if(command==nullptr)
        {
            return;
        }

        if(sentinel::string::equals_ignore_case(command, "help"))
        {
            execute_help();
        }
        else if(sentinel::string::equals_ignore_case(command, "clear"))
        {
            execute_clear();
        }
        else if(sentinel::string::equals_ignore_case(command, "version"))
        {
            execute_version();
        }
        else if(sentinel::string::starts_with_ignore_case(command, "echo "))
        {
            execute_echo(command);
        }
        else if(sentinel::string::equals_ignore_case(command, "panic-test"))
        {
            execute_panic_test();
        }
        else if(sentinel::string::equals_ignore_case(command, "idt-test"))
        {
            execute_idt_test();
        }
        else if(sentinel::string::equals_ignore_case(command, "memstats"))
        {
            execute_memstats();
        }
        else if(sentinel::string::equals_ignore_case(command, "alloc-page"))
        {
            execute_alloc_page();
        }
        else if(sentinel::string::equals_ignore_case(command, "free-page"))
        {
            execute_free_page();
        }
        else if(sentinel::string::equals_ignore_case(command, "memmap"))
        {
            execute_memmap();
        }
        else if(sentinel::string::equals_ignore_case(command, "reserved"))
        {
            execute_reserved();
        }
        else if(sentinel::string::starts_with_ignore_case(command, "page-info"))
        {
            char argStr[64];
            sentinel::string::substr(command, argStr, 9);

            sentinel::u64 arg=0;

            if(!sentinel::string::parse_u64(argStr, &arg))
            {
                sentinel::terminal::writeln("Invalid argument");
                return;
            }

            execute_page_info(arg);
        }
        else if(sentinel::string::starts_with_ignore_case(command, "alloc-contig"))
        {
            char argStr[64];
            sentinel::string::substr(command, argStr, 12);

            sentinel::u64 arg=0;
            
            if(!sentinel::string::parse_u64(argStr, &arg))
            {
                sentinel::terminal::writeln("Invalid argument");
                return;
            }

            execute_alloc_contig(arg);
        }
        else if(sentinel::string::equals_ignore_case(command, "free-contig"))
        {
            execute_free_contig();
        }
        else if(sentinel::string::equals_ignore_case(command, "heap-info"))
        {
            execute_heap_info();
        }
        else
        {
            command_not_found();
        }
    }

    static void execute_help()
    {
        sentinel::terminal::writeln("HELP:                    Pulls up this message with all valid commands");
        sentinel::terminal::writeln("CLEAR:                   Clears the console of all previous text");
        sentinel::terminal::writeln("VERSION:                 Prints out the current SentinelOS version");
        sentinel::terminal::writeln("ECHO:                    Displays messages");
        sentinel::terminal::writeln("PANIC-TEST:              Performs a panic test");
        sentinel::terminal::writeln("IDT-TEST:                Performs an idt test");
        sentinel::terminal::writeln("MEMSTATS:                Prints physical page allocator stats");
        sentinel::terminal::writeln("ALLOC-PAGE:              Allocates one test physical page");
        sentinel::terminal::writeln("FREE-PAGE:               Frees the last allocated test page");
        sentinel::terminal::writeln("MEMMAP                   Prints usable physical memory regions");
        sentinel::terminal::writeln("RESERVED                 Prints reserved physical memory regions");
        sentinel::terminal::writeln("PAGE-INFO <address>      Prints information regarding the page at a specified address");
        sentinel::terminal::writeln("ALLOC-CONTIG <count>     Allocates contiguous pages of a specified length and prints the starting address and length");
        sentinel::terminal::writeln("FREE-CONTIG              Frees contiguous pages at the address given in the previous ALLOC-CONTIG command, and prints the starting address and length");
        sentinel::terminal::writeln("HEAP-INFO                Prints kernel heap information");
    }

    static void execute_clear()
    {
        sentinel::terminal::clear();
    }

    static void execute_version()
    {
        sentinel::terminal::write("SentinelOS Version: ");
        sentinel::terminal::writeln(sentinel::terminal::get_version());
    }

    static void execute_echo(const char* command)
    {
        for(int i=5; command[i]!='\0'; i++)
        {
            sentinel::terminal::putchar(command[i]);
        }

        sentinel::terminal::putchar('\n');
    }

    static void execute_panic_test()
    {
        sentinel::panic("Performing Panic Test");
    }

    static void execute_idt_test()
    {
        sentinel::terminal::writeln("Triggering IDT test: Invalid opcode exception");

        __asm__ volatile("ud2");

        sentinel::terminal::writeln("IDT test returned");
    }

    static void execute_memstats()
    {
        sentinel::memory::physical_page_allocator::print_stats();
    }

    static void execute_alloc_page()
    {
        last_test_page=sentinel::memory::physical_page_allocator::allocate_page();

        if(last_test_page==0)
        {
            sentinel::terminal::writeln("Failed to allocate page");
            return;
        }

        sentinel::terminal::write("Allocated page: ");
        sentinel::terminal::writeln_hex(last_test_page);
    }

    static void execute_free_page()
    {
        if(last_test_page==0)
        {
            sentinel::terminal::writeln("No test page allocated");
            return;
        }

        sentinel::memory::physical_page_allocator::free_page(last_test_page);

        sentinel::terminal::write("Freed page: ");
        sentinel::terminal::writeln_hex(last_test_page);

        last_test_page=0;
    }

    static void execute_memmap()
    {
        const sentinel::memory::PhysicalMemoryRegion* usable_regions
                =sentinel::memory::get_usable_regions();
        
        sentinel::u32 usable_region_count=sentinel::memory::get_usable_region_count();
    
        sentinel::terminal::writeln("Usable Memory Regions:");
        for(sentinel::u32 i=0; i<usable_region_count; i++)
        {
            sentinel::terminal::write("Region ");
            sentinel::terminal::write_u64(i);
            sentinel::terminal::writeln(":");

            sentinel::terminal::write("Start: ");
            sentinel::terminal::writeln_u64(usable_regions[i].start);
            
            sentinel::terminal::write("End: ");
            sentinel::terminal::writeln_u64(usable_regions[i].end);

            sentinel::terminal::write("Size KiB: ");
            sentinel::u64 sizeKiB=usable_regions[i].length/1024;
            sentinel::terminal::writeln_u64(sizeKiB);

            sentinel::terminal::writeln("");
        }
    }

    static void execute_reserved()
    {
        sentinel::memory::print_reserved_regions();
    }

    static void execute_page_info(sentinel::u64 address)
    {
        sentinel::terminal::write("Page address: ");
        sentinel::u64 page_address=sentinel::memory::align_page_down(address);
        sentinel::terminal::writeln_hex(page_address);

        sentinel::terminal::write("Owned by allocator: ");
        bool owned=sentinel::memory::physical_page_allocator::owns_page(page_address);

        if(owned)
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
            return;
        }

        sentinel::terminal::write("Used: ");
        bool used=sentinel::memory::physical_page_allocator::is_page_used(page_address);

        if(used)
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
        }

        sentinel::terminal::write("Free: ");
        bool free=sentinel::memory::physical_page_allocator::is_page_free(page_address);

        if(free)
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
        }

        sentinel::terminal::write("Reserved: ");
        bool reserved=
            sentinel::memory::physical_page_allocator::is_reserved_page(page_address);

        if(reserved)
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
        }

        sentinel::terminal::write("Allocated metadata: ");
        bool allocated=
            sentinel::memory::physical_page_allocator::is_metadata_allocated(page_address);

        if(allocated)
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
        }
    }

    static void execute_alloc_contig(sentinel::u64 count)
    {
        last_contiguous_page=
            sentinel::memory::physical_page_allocator::
                allocate_contiguous_pages(count);

        last_contiguous_count=count;

        if(last_contiguous_page==0)
        {
            last_contiguous_count=0;
            sentinel::terminal::writeln("Failed to allocate contiguous pages");
            return;
        }

        sentinel::terminal::write("Allocated ");
        sentinel::terminal::writeln_u64(last_contiguous_count);
        sentinel::terminal::write(" contiguous pages at: ");
        sentinel::terminal::writeln_hex(last_contiguous_page);
    }

    static void execute_free_contig()
    {
        if(last_contiguous_count==0 || last_contiguous_page==0)
        {
            sentinel::terminal::writeln("No test page allocated");
            return;
        }

        sentinel::memory::physical_page_allocator::
            free_contiguous_pages(last_contiguous_page, last_contiguous_count);

        sentinel::terminal::write("Freed ");
        sentinel::terminal::writeln_u64(last_contiguous_count);
        sentinel::terminal::write(" contiguous pages at: ");
        sentinel::terminal::writeln_hex(last_contiguous_page);

        last_contiguous_page=0;
        last_contiguous_count=0;
    }

    static void execute_heap_info()
    {
        sentinel::memory::kernel_heap::print_heap_info();
    }

    static void command_not_found()
    {
        sentinel::terminal::writeln("Command not found");
    }
}