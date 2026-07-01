#pragma once

#include <sentinel/types.hpp>

namespace sentinel::string
{
    sentinel::u64 length(const char* str);

    bool equals(const char* str1, const char* str2);

    bool equals_ignore_case(const char* str1, const char* str2);

    void copy(char* dest, const char* src);

    void concat(char* dest, const char* src);

    bool isEmpty(const char* str);

    bool starts_with(const char* str, const char* substr);

    bool starts_with_ignore_case(const char* str, const char* substr);

    void remove_last(char* str);

    void clear(char* str);
}