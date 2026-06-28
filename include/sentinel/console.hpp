#pragma once

#include <sentinel/drivers/keyboard.hpp>

namespace sentinel::console
{
    void initialize();

    void handle_key_event(const sentinel::drivers::keyboard::KeyEvent& event);

    bool has_line();

    bool read_line(char* out_buffer);
}