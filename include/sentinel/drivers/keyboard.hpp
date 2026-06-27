#pragma once

#include <sentinel/types.hpp>

namespace sentinel::drivers::keyboard
{
    struct KeyEvent
    {
        sentinel::u8 scancode;
        char ascii;
        bool pressed;
    };

    void initialize();

    bool has_event();

    bool read_event(KeyEvent& event);
}