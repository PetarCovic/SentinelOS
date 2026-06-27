#include <sentinel/arch/x86_64/pic.hpp>
#include <sentinel/arch/x86_64/io.hpp>

namespace sentinel::arch::x86_64::pic
{
    static constexpr sentinel::u16 PIC1_COMMAND=0x20;
    static constexpr sentinel::u16 PIC1_DATA=0x21;
    static constexpr sentinel::u16 PIC2_COMMAND=0xA0;
    static constexpr sentinel::u16 PIC2_DATA=0xA1;

    static constexpr sentinel::u8 PIC_EOI=0x20;

    static constexpr sentinel::u8 ICW1_INIT=0x10;
    static constexpr sentinel::u8 ICW1_ICW4=0x01;
    static constexpr sentinel::u8 ICW4_8086=0x01;

    static constexpr sentinel::u8 MASTER_OFFSET=0x20;
    static constexpr sentinel::u8 SLAVE_OFFSET=0x28;

    void remap()
    {
        //Start PIC initialization sequence
        io::outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
        io::io_wait();

        io::outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
        io::io_wait();

        //Set vector offsets
        io::outb(PIC1_DATA, MASTER_OFFSET);
        io::io_wait();

        io::outb(PIC2_DATA, SLAVE_OFFSET);
        io::io_wait();

        //Tell master PIC that slave PIC is at IRQ2
        io::outb(PIC1_DATA, 0x04);
        io::io_wait();

        //Tell slave PIC its cascade identity
        io::outb(PIC2_DATA, 0x02);
        io::io_wait();

        //Use 8086/88 mode
        io::outb(PIC1_DATA, ICW4_8086);
        io::io_wait();

        io::outb(PIC2_DATA, ICW4_8086);
        io::io_wait();

        //Mask all hardware IRQs for now
        //TODO: Unmask keyboard IRQ1 later
        mask_all();
    }

    void send_eoi(sentinel::u8 irq)
    {
        if (irq >= 16)
        {
            return;
        }

        if(irq>=8)
        {
            io::outb(PIC2_COMMAND, PIC_EOI);
        }

        io::outb(PIC1_COMMAND, PIC_EOI);
    }

    void mask_irq(sentinel::u8 irq)
    {
        if (irq >= 16)
        {
            return;
        }

        sentinel::u16 port;

        if (irq < 8)
        {
            port = PIC1_DATA;
        }  
        else
        {
            port = PIC2_DATA;
            irq -= 8;
        }

        sentinel::u8 mask = static_cast<sentinel::u8>(1u << irq);
        sentinel::u8 value = io::inb(port) | mask;

        io::outb(port, value);
    }

    void unmask_irq(sentinel::u8 irq)
    {
        if (irq >= 16)
        {
            return;
        }

        sentinel::u16 port;

        if (irq < 8)
        {
            port = PIC1_DATA;
        }
        else
        {
            port = PIC2_DATA;
            irq -= 8;
        }

        sentinel::u8 mask = static_cast<sentinel::u8>(1u << irq);
        sentinel::u8 value = io::inb(port) & static_cast<sentinel::u8>(~mask);

        io::outb(port, value);
    }

    void mask_all()
    {
        io::outb(PIC1_DATA, 0xFF);
        io::outb(PIC2_DATA, 0xFF);
    }
}