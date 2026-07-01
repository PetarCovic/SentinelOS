#include <sentinel/drivers/keyboard.hpp>

#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/io.hpp>
#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/arch/x86_64/cpu.hpp>
#include <sentinel/types.hpp>

extern "C" void irq1_keyboard_stub();

namespace sentinel::drivers::keyboard
{
    static constexpr sentinel::u16 KEYBOARD_DATA_PORT = 0x60;

    static constexpr sentinel::u8 KEYBOARD_IRQ = 1;
    static constexpr sentinel::u8 KEYBOARD_VECTOR = 33;

    static constexpr sentinel::u8 INTERRUPT_GATE = 0x8E;

    static constexpr int EVENT_QUEUE_SIZE = 64;

    static KeyEvent event_queue[EVENT_QUEUE_SIZE];

    static int queue_head = 0;
    static int queue_tail = 0;
    static int queue_count = 0;

    static bool pending_extended = false;

    static bool left_shift_down = false;
    static bool right_shift_down = false;
    static bool caps_lock_on = false;

    static bool shift_down()
    {
        return left_shift_down || right_shift_down;
    }

    static bool is_letter_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            case 0x10: // q
            case 0x11: // w
            case 0x12: // e
            case 0x13: // r
            case 0x14: // t
            case 0x15: // y
            case 0x16: // u
            case 0x17: // i
            case 0x18: // o
            case 0x19: // p

            case 0x1E: // a
            case 0x1F: // s
            case 0x20: // d
            case 0x21: // f
            case 0x22: // g
            case 0x23: // h
            case 0x24: // j
            case 0x25: // k
            case 0x26: // l

            case 0x2C: // z
            case 0x2D: // x
            case 0x2E: // c
            case 0x2F: // v
            case 0x30: // b
            case 0x31: // n
            case 0x32: // m
                return true;

            default:
                return false;
        }
    }

    static bool is_printable_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            // Number row
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            case 0x0B:

            // Letters
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:

            case 0x1E:
            case 0x1F:
            case 0x20:
            case 0x21:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:

            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
            case 0x30:
            case 0x31:
            case 0x32:

            // Space and punctuation
            case 0x39:
            case 0x0C:
            case 0x0D:
            case 0x1A:
            case 0x1B:
            case 0x2B:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x33:
            case 0x34:
            case 0x35:
                return true;

            default:
                return false;
        }
    }

    static char lowercase_letter_from_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            case 0x10: return 'q';
            case 0x11: return 'w';
            case 0x12: return 'e';
            case 0x13: return 'r';
            case 0x14: return 't';
            case 0x15: return 'y';
            case 0x16: return 'u';
            case 0x17: return 'i';
            case 0x18: return 'o';
            case 0x19: return 'p';

            case 0x1E: return 'a';
            case 0x1F: return 's';
            case 0x20: return 'd';
            case 0x21: return 'f';
            case 0x22: return 'g';
            case 0x23: return 'h';
            case 0x24: return 'j';
            case 0x25: return 'k';
            case 0x26: return 'l';

            case 0x2C: return 'z';
            case 0x2D: return 'x';
            case 0x2E: return 'c';
            case 0x2F: return 'v';
            case 0x30: return 'b';
            case 0x31: return 'n';
            case 0x32: return 'm';

            default:
                return 0;
        }
    }

    static char unshifted_ascii_from_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            // Number row
            case 0x02: return '1';
            case 0x03: return '2';
            case 0x04: return '3';
            case 0x05: return '4';
            case 0x06: return '5';
            case 0x07: return '6';
            case 0x08: return '7';
            case 0x09: return '8';
            case 0x0A: return '9';
            case 0x0B: return '0';

            // Punctuation
            case 0x0C: return '-';
            case 0x0D: return '=';
            case 0x1A: return '[';
            case 0x1B: return ']';
            case 0x2B: return '\\';
            case 0x27: return ';';
            case 0x28: return '\'';
            case 0x29: return '`';
            case 0x33: return ',';
            case 0x34: return '.';
            case 0x35: return '/';

            // Space
            case 0x39: return ' ';

            default:
                return lowercase_letter_from_scancode(scancode);
        }
    }

    static char shifted_ascii_from_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            // Number row shifted symbols
            case 0x02: return '!';
            case 0x03: return '@';
            case 0x04: return '#';
            case 0x05: return '$';
            case 0x06: return '%';
            case 0x07: return '^';
            case 0x08: return '&';
            case 0x09: return '*';
            case 0x0A: return '(';
            case 0x0B: return ')';

            // Shifted punctuation
            case 0x0C: return '_';
            case 0x0D: return '+';
            case 0x1A: return '{';
            case 0x1B: return '}';
            case 0x2B: return '|';
            case 0x27: return ':';
            case 0x28: return '"';
            case 0x29: return '~';
            case 0x33: return '<';
            case 0x34: return '>';
            case 0x35: return '?';

            default:
            {
                char letter = lowercase_letter_from_scancode(scancode);

                if (letter >= 'a' && letter <= 'z')
                {
                    return static_cast<char>(letter - 'a' + 'A');
                }

                return 0;
            }
        }
    }

    static char translate_ascii_scancode(sentinel::u8 scancode)
    {
        if (!is_printable_scancode(scancode))
        {
            return 0;
        }

        if (is_letter_scancode(scancode))
        {
            char letter = lowercase_letter_from_scancode(scancode);

            bool uppercase = shift_down() ^ caps_lock_on;

            if (uppercase)
            {
                return static_cast<char>(letter - 'a' + 'A');
            }

            return letter;
        }

        if (shift_down())
        {
            return shifted_ascii_from_scancode(scancode);
        }

        return unshifted_ascii_from_scancode(scancode);
    }

    static KeyCode translate_normal_key_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            case 0x01: return KeyCode::ESCAPE;
            case 0x1C: return KeyCode::ENTER;
            case 0x0E: return KeyCode::BACKSPACE;
            case 0x0F: return KeyCode::TAB;

            case 0x2A: return KeyCode::LEFT_SHIFT;
            case 0x36: return KeyCode::RIGHT_SHIFT;
            case 0x3A: return KeyCode::CAPS_LOCK;
            case 0x1D: return KeyCode::LEFT_CTRL;
            case 0x38: return KeyCode::LEFT_ALT;

            default:
                if (is_printable_scancode(scancode))
                {
                    return KeyCode::CHARACTER;
                }

                return KeyCode::UNKNOWN;
        }
    }

    static KeyCode translate_extended_key_scancode(sentinel::u8 scancode)
    {
        switch (scancode)
        {
            case 0x4B: return KeyCode::LEFT_ARROW;
            case 0x4D: return KeyCode::RIGHT_ARROW;
            case 0x48: return KeyCode::UP_ARROW;
            case 0x50: return KeyCode::DOWN_ARROW;

            case 0x47: return KeyCode::HOME;
            case 0x4F: return KeyCode::END;
            case 0x52: return KeyCode::INSERT;
            case 0x53: return KeyCode::DELETE;

            default:
                return KeyCode::UNKNOWN;
        }
    }

    void initialize()
    {
        pending_extended = false;

        left_shift_down = false;
        right_shift_down = false;
        caps_lock_on = false;

        queue_head = 0;
        queue_tail = 0;
        queue_count = 0;

        sentinel::arch::x86_64::idt::set_entry(
            KEYBOARD_VECTOR,
            irq1_keyboard_stub,
            0,
            INTERRUPT_GATE
        );

        sentinel::arch::x86_64::pic::unmask_irq(KEYBOARD_IRQ);
    }

    static void push_event(const KeyEvent& event)
    {
        if (queue_count >= EVENT_QUEUE_SIZE)
        {
            return;
        }

        event_queue[queue_tail] = event;
        queue_tail = (queue_tail + 1) % EVENT_QUEUE_SIZE;
        queue_count++;
    }

    bool has_event()
    {
        sentinel::u64 curr_rflags =
            sentinel::arch::x86_64::cpu::read_rflags();

        sentinel::arch::x86_64::cpu::disable_interrupts();

        bool result = queue_count > 0;

        sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);

        return result;
    }

    bool read_event(KeyEvent& event)
    {
        sentinel::u64 curr_rflags =
            sentinel::arch::x86_64::cpu::read_rflags();

        sentinel::arch::x86_64::cpu::disable_interrupts();

        if (queue_count == 0)
        {
            sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);
            return false;
        }

        event = event_queue[queue_head];
        queue_head = (queue_head + 1) % EVENT_QUEUE_SIZE;
        queue_count--;

        sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);

        return true;
    }

    void handle_raw_scancode(sentinel::u8 raw_scancode)
    {
        if (raw_scancode == 0xE0)
        {
            pending_extended = true;
            return;
        }

        bool released = (raw_scancode & 0x80) != 0;
        bool pressed = !released;

        sentinel::u8 clean_scancode =
            static_cast<sentinel::u8>(raw_scancode & 0x7F);

        bool extended = pending_extended;

        KeyCode key = KeyCode::UNKNOWN;
        char ascii = 0;

        if (extended)
        {
            key = translate_extended_key_scancode(clean_scancode);
            ascii = 0;
            pending_extended = false;
        }
        else
        {
            key = translate_normal_key_scancode(clean_scancode);

            if (key == KeyCode::LEFT_SHIFT)
            {
                left_shift_down = pressed;
            }
            else if (key == KeyCode::RIGHT_SHIFT)
            {
                right_shift_down = pressed;
            }
            else if (key == KeyCode::CAPS_LOCK && pressed)
            {
                caps_lock_on = !caps_lock_on;
            }

            if (pressed && key == KeyCode::CHARACTER)
            {
                ascii = translate_ascii_scancode(clean_scancode);
            }
            else if (pressed && key == KeyCode::ENTER)
            {
                ascii = '\n';
            }
            else if (pressed && key == KeyCode::BACKSPACE)
            {
                ascii = '\b';
            }
            else if (pressed && key == KeyCode::TAB)
            {
                ascii = '\t';
            }
        }

        if (key == KeyCode::UNKNOWN && ascii == 0)
        {
            return;
        }

        KeyEvent event;
        event.scancode = clean_scancode;
        event.ascii = ascii;
        event.pressed = pressed;
        event.extended = extended;
        event.key = key;

        push_event(event);
    }
}

extern "C" void keyboard_interrupt_handler()
{
    sentinel::u8 raw_scancode =
        sentinel::arch::x86_64::io::inb(
            sentinel::drivers::keyboard::KEYBOARD_DATA_PORT
        );

    sentinel::drivers::keyboard::handle_raw_scancode(raw_scancode);

    sentinel::arch::x86_64::pic::send_eoi(
        sentinel::drivers::keyboard::KEYBOARD_IRQ
    );
}