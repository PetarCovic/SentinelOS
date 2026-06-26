#pragma once

namespace sentinel::terminal
{
    void initialize();

    void putchar(char c);

    void write(const char* text);

    void writeln(const char* text);

    void clear();

    int get_cursor_row();

    int get_cursor_col();

    void set_cursor_row(int row);

    void set_cursor_col(int col);

    void set_cursor(int row, int col);
}