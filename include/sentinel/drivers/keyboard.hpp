#pragma once

#include <sentinel/types.hpp>

namespace sentinel::drivers::keyboard
{
    enum class KeyCode
    {
        UNKNOWN,
        CHARACTER,

        ESCAPE,
        ENTER,
        BACKSPACE,
        TAB,

        LEFT_ARROW,
        RIGHT_ARROW,
        UP_ARROW,
        DOWN_ARROW,

        HOME,
        END,
        INSERT,
        DELETE,

        LEFT_SHIFT,
        RIGHT_SHIFT,
        CAPS_LOCK,
        LEFT_CTRL,
        LEFT_ALT
    };

    struct KeyEvent
    {
        sentinel::u8 scancode;
        char ascii;
        bool pressed;
        bool extended;
        KeyCode key;
    };

    void initialize();

    bool has_event();
    
    bool read_event(KeyEvent& event);
}