#pragma once

#include <sentinel/types.hpp>

namespace sentinel::terminal
{
    void initialize();

    void putchar(char c);

    void backspace();

    void write(const char* text);

    void writeln(const char* text);

    void clear();

    sentinel::u16 get_color();

    int get_cursor_row();

    int get_cursor_col();

    char* get_version();

    void set_color(sentinel::u16 color);

    void set_cursor_row(int row);

    void set_cursor_col(int col);

    void set_cursor(int row, int col);
}