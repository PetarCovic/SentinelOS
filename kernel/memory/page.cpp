#include <sentinel/memory/page.hpp>
#include <sentinel/types.hpp>

namespace sentinel::memory
{
    sentinel::u64 align_down(sentinel::u64 address, sentinel::u64 alignment)
    {
        return address-(address%alignment);
    }

    sentinel::u64 align_up(sentinel::u64 address, sentinel::u64 alignment)
    {
        sentinel::u64 remainder=address%alignment;

        if(remainder==0)
        {
            return address;
        }

        return address+(alignment-remainder);
    }

    sentinel::u64 align_page_down(sentinel::u64 address)
    {
        return align_down(address, PAGE_SIZE);
    }

    sentinel::u64 align_page_up(sentinel::u64 address)
    {
        return align_up(address, PAGE_SIZE);
    }

    bool is_aligned(sentinel::u64 address, sentinel::u64 alignment)
    {
        if(address%alignment==0)
        {
            return true;
        }

        return false;
    }

    bool is_page_aligned(sentinel::u64 address)
    {
        if(address%PAGE_SIZE==0)
        {
            return true;
        }

        return false;
    }
}