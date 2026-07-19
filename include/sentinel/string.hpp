#pragma once

#include <sentinel/types.hpp>

namespace sentinel::string
{
    sentinel::u64 length(const char* str);

    bool equals(const char* str1, const char* str2);

    bool equals_ignore_case(const char* str1, const char* str2);

    void copy(char* dest, const char* src);

    void concat(char* dest, const char* src);

    bool is_empty(const char* str);

    bool starts_with(const char* str, const char* substr);

    bool starts_with_ignore_case(const char* str, const char* substr);

    void substr(const char* str, char* dst, sentinel::u32 beginIndex);

    void substr(const char* str, char* dst, sentinel::u32 beginIndex, sentinel::u32 endIndex);

    bool parse_u64(const char* str, sentinel::u64* out_value);

    void remove_last(char* str);

    void clear(char* str);
}