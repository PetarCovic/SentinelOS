#pragma once

#include <sentinel/types.hpp>

namespace sentinel::string
{
    sentinel::u64 length(const char* str);

    bool equals(char* str1, const char* str2);

    void copy(char* dest, const char* src);

    void concat(char* dest, const char* src);

    bool starts_with(const char* str, const char* substr);

    void remove_last(char* str);

    void clear(char* str);
}