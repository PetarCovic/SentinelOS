#include <sentinel/memory/kernel_heap.hpp>
#include <sentinel/types.hpp>
#include <sentinel/terminal.hpp>

namespace sentinel::memory::kernel_heap
{
    static bool heap_initialized=false;

    static constexpr sentinel::u64 HEAP_START=0xFFFF800000000000;
    static constexpr sentinel::u64 HEAP_SIZE=16*1024*1024;
    static constexpr sentinel::u64 HEAP_END=HEAP_START+HEAP_SIZE;

    void initialize()
    {
        heap_initialized=true;
    }

    bool is_initialized()
    {
        return heap_initialized;
    }

    sentinel::u64 get_heap_start()
    {
        return HEAP_START;
    }

    sentinel::u64 get_heap_end()
    {
        return HEAP_END;
    }

    sentinel::u64 get_heap_size()
    {
        return HEAP_SIZE;
    }

    void print_heap_info()
    {
        sentinel::terminal::write("Heap start: ");
        sentinel::terminal::writeln_hex(get_heap_start());

        sentinel::terminal::write("Heap end: ");
        sentinel::terminal::writeln_hex(get_heap_end());

        sentinel::terminal::write("Heap size: ");
        sentinel::terminal::writeln_hex(get_heap_size());

        sentinel::terminal::write("Heap initialized: ");
        
        if(is_initialized())
        {
            sentinel::terminal::writeln("Yes");
        }
        else
        {
            sentinel::terminal::writeln("No");
        }
    }
}