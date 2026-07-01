#include <sentinel/drivers/keyboard.hpp>

#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/arch/x86_64/io.hpp>
#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/arch/x86_64/cpu.hpp>
#include <sentinel/terminal.hpp>
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

    static int queue_head=0;
    static int queue_tail=0;
    static int queue_count=0;

    static char translate_scancode(sentinel::u8 scancode)
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

            // Top letter row
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

            // Home row
            case 0x1E: return 'a';
            case 0x1F: return 's';
            case 0x20: return 'd';
            case 0x21: return 'f';
            case 0x22: return 'g';
            case 0x23: return 'h';
            case 0x24: return 'j';
            case 0x25: return 'k';
            case 0x26: return 'l';

            // Bottom row
            case 0x2C: return 'z';
            case 0x2D: return 'x';
            case 0x2E: return 'c';
            case 0x2F: return 'v';
            case 0x30: return 'b';
            case 0x31: return 'n';
            case 0x32: return 'm';

            // Basic control keys
            case 0x39: return ' ';  // Space
            case 0x1C: return '\n'; // Enter
            case 0x0E: return '\b'; // Backspace
            //case 0x2A: return;      // LShift
            //case 0x32: return;      // RShift
            //arrows. left and right to move cursor, up and down to go 
            //through commands
            //Go through and allow for every key to have functionality. Or at least
            //Every common key

            default:
                return 0;
        }
    }

    void initialize()
    {
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
        if(queue_count>=EVENT_QUEUE_SIZE)
        {
            return;
        }

        event_queue[queue_tail]=event;
        queue_tail=(queue_tail+1)%EVENT_QUEUE_SIZE;
        queue_count++;
    }

    bool has_event()
    {
        sentinel::u64 curr_rflags=sentinel::arch::x86_64::cpu::read_rflags();

        sentinel::arch::x86_64::cpu::disable_interrupts();

        bool result=queue_count>0;

        sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);

        return result;
    }

    bool read_event(KeyEvent& event)
    {
        sentinel::u64 curr_rflags=sentinel::arch::x86_64::cpu::read_rflags();
        sentinel::arch::x86_64::cpu::disable_interrupts();

        if(queue_count==0)
        {
            sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);
            return false;
        }
        
        event=event_queue[queue_head];
        queue_head=(queue_head+1)%EVENT_QUEUE_SIZE;
        queue_count--;

        sentinel::arch::x86_64::cpu::restore_rflags(curr_rflags);

        return true;
    }
}

extern "C" void keyboard_interrupt_handler()
{
    sentinel::u8 scancode =
        sentinel::arch::x86_64::io::inb(
            sentinel::drivers::keyboard::KEYBOARD_DATA_PORT);

    bool released = (scancode & 0x80) != 0;
    bool pressed=!released;

    sentinel::u8 clean_scancode=static_cast<sentinel::u8>(scancode & 0x7F);

    char ascii=sentinel::drivers::keyboard::translate_scancode(clean_scancode);

    sentinel::drivers::keyboard::KeyEvent event;
    event.scancode=clean_scancode;
    event.ascii=ascii;
    event.pressed=pressed;

    sentinel::drivers::keyboard::push_event(event);

    sentinel::arch::x86_64::pic::send_eoi(
        sentinel::drivers::keyboard::KEYBOARD_IRQ);
}