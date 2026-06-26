#include <sentinel/terminal.hpp>
#include <sentinel/logger.hpp>

namespace sentinel::logger
{
    void log_info(const char* message)
    {
        sentinel::u16 original_color=sentinel::terminal::get_color();

        sentinel::terminal::set_color(0x0F);
        sentinel::terminal::writeln(message);
        sentinel::terminal::set_color(original_color);
    }

    void log_warn(const char* message)
    {
        sentinel::u16 original_color=sentinel::terminal::get_color();

        sentinel::terminal::set_color(0x0E);
        sentinel::terminal::writeln(message);
        sentinel::terminal::set_color(original_color);
    }

    void log_error(const char* message)
    {
        sentinel::u16 original_color=sentinel::terminal::get_color();

        sentinel::terminal::set_color(0x04);
        sentinel::terminal::writeln(message);
        sentinel::terminal::set_color(original_color);
    }
}