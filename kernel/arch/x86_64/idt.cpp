#include <sentinel/arch/x86_64/idt.hpp>
#include <sentinel/types.hpp>
#include <sentinel/panic.hpp>

extern "C" void default_interrupt_stub();

extern "C" void isr0_stub();
extern "C" void isr6_stub();
extern "C" void isr13_stub();
extern "C" void isr14_stub();

namespace sentinel::arch::x86_64::idt
{
    struct __attribute__((packed)) IDTEntry
    {
        sentinel::u16 offset_low;
        sentinel::u16 selector;
        sentinel::u8  ist;
        sentinel::u8  type_attr;
        sentinel::u16 offset_mid;
        sentinel::u32 offset_high;
        sentinel::u32 zero;
    };

    struct __attribute__((packed)) IDTR
    {
        sentinel::u16 limit;
        sentinel::u64 base;
    };

    static IDTEntry idt[256];
    static IDTR idtr;

    static constexpr sentinel::u16 KERNEL_CODE_SELECTOR = 0x18;
    static constexpr sentinel::u8 INTERRUPT_GATE = 0x8E;

    void set_entry(int index, void (*handler)(), unsigned char ist, unsigned char type_attr)
    {
        sentinel::u64 addr = reinterpret_cast<sentinel::u64>(handler);

        idt[index].offset_low  = addr & 0xFFFF;
        idt[index].selector    = KERNEL_CODE_SELECTOR;
        idt[index].ist         = ist & 0x07;
        idt[index].type_attr   = type_attr;
        idt[index].offset_mid  = (addr >> 16) & 0xFFFF;
        idt[index].offset_high = (addr >> 32) & 0xFFFFFFFF;
        idt[index].zero        = 0;
    }

    void initialize()
    {
        for (int i = 0; i < 256; i++)
        {
            idt[i] = {};
        }

        for (int i = 0; i < 256; i++)
        {
            set_entry(i, default_interrupt_stub, 0, INTERRUPT_GATE);
        }

        set_entry(0,  isr0_stub,  0, INTERRUPT_GATE);
        set_entry(6,  isr6_stub,  0, INTERRUPT_GATE);
        set_entry(13, isr13_stub, 0, INTERRUPT_GATE);
        set_entry(14, isr14_stub, 0, INTERRUPT_GATE);

        idtr.limit = sizeof(idt) - 1;
        idtr.base = reinterpret_cast<sentinel::u64>(&idt);

        load();
    }

    void load()
    {
        __asm__ volatile ("lidt %0" : : "m"(idtr));
    }
}