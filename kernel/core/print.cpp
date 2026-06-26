#include <sentinel/print.hpp>
#include<sentinel/terminal.hpp>

namespace sentinel
{
    void print(const char* text)
    {
        terminal::write(text);
    }

    void println(const char* text)
    {
        terminal::writeln(text);
    }
}