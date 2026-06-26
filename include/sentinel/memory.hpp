#pragma once

namespace sentinel::memory
{
    void* memset(void* dest, int value, unsigned long count);

    void* memcpy(void* dest, const void* src, unsigned long count);

    unsigned long strlen(const char* str);
}