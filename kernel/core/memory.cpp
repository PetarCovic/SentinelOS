#include <sentinel/memory.hpp>

namespace sentinel::memory
{
    void* memset(void* dest, int value, unsigned long count)
    {
        unsigned char* d = static_cast<unsigned char*>(dest);

        for(int i=0; i<count; i++)
        {
            d[i]=value;
        }

        return dest;
    }

    void* memcpy(void* dest, const void* src, unsigned long count)
    {
        unsigned char* d = static_cast<unsigned char*>(dest);
        const unsigned char* s = static_cast<const unsigned char*>(src);

        for(int i=0; i<count; i++)
        {
            d[i]=s[i];
        }

        return dest;
    }

    unsigned long strlen(const char* str)
    {
        unsigned long count=0;

        for(int i=0; str[i]!='\0'; i++)
        {
            count++;
        }

        return count;
    }
}