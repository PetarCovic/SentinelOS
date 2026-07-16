#include <sentinel/terminal.hpp>
#include <sentinel/types.hpp>
#include <sentinel/arch/x86_64/io.hpp>

namespace sentinel::terminal
{
    static void clear_row(sentinel::u64 row);
    static void scroll_up();
    static void advance_cursor();

    static int cursor_row;
    static int cursor_col;

    static constexpr int VGA_WIDTH = 80;
    static constexpr int VGA_HEIGHT = 25;
    static constexpr sentinel::u16 DEFAULT_COLOR = 0x0F;
    static sentinel::u16 vga_color = DEFAULT_COLOR;
    static volatile sentinel::u16* const VGA_BUFFER =
        (volatile sentinel::u16*)0xB8000;

    static constexpr const char* VERSION="0.1";

    void initialize()
    {
        clear();
    }

    void putchar(char c)
    {
        if(c=='\n')
        {
            cursor_col=0;
            cursor_row++;

            if(cursor_row>=VGA_HEIGHT)
            {
                scroll_up();
            }

            set_cursor(cursor_row, cursor_col);

            return;
        }
        
        int index=cursor_row*VGA_WIDTH+cursor_col;

        VGA_BUFFER[index] = (vga_color << 8) | static_cast<sentinel::u16>(c);
        
        advance_cursor();
        set_cursor(cursor_row, cursor_col);
    }

    void backspace()
    {
        if(cursor_row==0 && cursor_col==0)
        {
            return;
        }

        if(cursor_col>0)
        {
            set_cursor(cursor_row, cursor_col-1);
        }
        else
        {
            set_cursor(cursor_row-1, VGA_WIDTH-1);
        }


        int index=cursor_row*VGA_WIDTH+cursor_col;

        VGA_BUFFER[index] = (vga_color << 8) | static_cast<sentinel::u16>(' ');
    }

    void write(const char* text)
    {
        if (text == nullptr)
        {
            return;
        }
        
        for(int i=0; text[i]!='\0'; i++)
        {
            putchar(text[i]);
        }
    }

    void writeln(const char* text)
    {
        write(text);
        putchar('\n');
    }

    void write_u64(sentinel::u64 number)
    {
        char buffer[21]; // max u64 decimal digits = 20, plus null terminator
        int index = 0;

        if(number == 0)
        {
            putchar('0');
            return;
        }

        while(number > 0)
        {
            sentinel::u64 digit = number % 10;
            buffer[index] = static_cast<char>('0' + digit);
            index++;
            number /= 10;
        }

        for(int i = index - 1; i >= 0; i--)
        {
            putchar(buffer[i]);
        }
    }

    void writeln_u64(sentinel::u64 number)
    {
        write_u64(number);
        putchar('\n');
    }

    void write_i64(sentinel::i64 number)
    {
        if(number < 0)
        {
            putchar('-');
            write_u64(static_cast<sentinel::u64>(-number));
            return;
        }

        write_u64(static_cast<sentinel::u64>(number));
    }

    void writeln_i64(sentinel::i64 number)
    {
        write_i64(number);
        putchar('\n');
    }

    void write_hex(sentinel::u64 number)
    {
        static constexpr char HEX_DIGITS[] = "0123456789ABCDEF";

        write("0x");

        bool started = false;

        for(int shift = 60; shift >= 0; shift -= 4)
        {
            sentinel::u8 nibble =
                static_cast<sentinel::u8>((number >> shift) & 0xF);

            if(nibble != 0 || started || shift == 0)
            {
                putchar(HEX_DIGITS[nibble]);
                started = true;
            }
        }
    }

    void writeln_hex(sentinel::u64 number)
    {
        write_hex(number);
        putchar('\n');
    }

    void clear()
    {
        for(int r=0; r<VGA_HEIGHT; r++)
        {
            for(int c=0; c<VGA_WIDTH; c++)
            {
                int index=r*VGA_WIDTH+c;

                VGA_BUFFER[index] = (vga_color << 8) | ' ';            
            }
        }

        set_cursor(0, 0);
    }

    sentinel::u16 get_color()
    {
        return vga_color;
    }

    int get_cursor_row()
    {
        return cursor_row;
    }

    int get_cursor_col()
    {
        return cursor_col;
    }

    const char* get_version()
    {
        return VERSION;
    }

    void set_color(sentinel::u16 color)
    {
        vga_color=color & 0xFF;
    }

    void set_cursor(int row, int col)
    {
        if (row < 0)
        {
            row = 0;
        }

        if (row >= VGA_HEIGHT)
        {
            row = VGA_HEIGHT - 1;
        }

        if (col < 0)
        {
            col = 0;
        }

        if (col >= VGA_WIDTH)
        {
            col = VGA_WIDTH - 1;
        }

        cursor_row = row;
        cursor_col = col;

        sentinel::u16 position=
            static_cast<sentinel::u16>(cursor_row*VGA_WIDTH+cursor_col);

        static constexpr sentinel::u16 VGA_CONTROL_PORT=0x3D4;   //VGA cursor control port
        static constexpr sentinel::u16 VGA_DATA_PORT=0x3D5;      //VGA cursor data port

        static constexpr sentinel::u16 VGA_CURSOR_LOW_REGISTER=0x0F;
        static constexpr sentinel::u16 VGA_CURSOR_HIGH_REGISTER=0x0E;

        //Low byte
        sentinel::arch::x86_64::io::outb(VGA_CONTROL_PORT, VGA_CURSOR_LOW_REGISTER);
        sentinel::arch::x86_64::io::outb(VGA_DATA_PORT, 
            static_cast<sentinel::u8>(position & 0xFF));

        //High byte
        sentinel::arch::x86_64::io::outb(VGA_CONTROL_PORT, VGA_CURSOR_HIGH_REGISTER);
        sentinel::arch::x86_64::io::outb(VGA_DATA_PORT, 
            static_cast<sentinel::u8>((position >> 8) & 0xFF));
    }

    static void clear_row(sentinel::u64 row)
    {
        for(sentinel::u64 col=0; col<VGA_WIDTH; col++)
        {
            VGA_BUFFER[row*VGA_WIDTH+col]=(vga_color << 8) | ' ';  
        }
    }

    static void scroll_up()
    {
        for(sentinel::u64 row=1; row<VGA_HEIGHT; row++)
        {
            for(sentinel::u64 col=0; col<VGA_WIDTH; col++)
            {
                VGA_BUFFER[(row-1)*VGA_WIDTH+col]=VGA_BUFFER[row*VGA_WIDTH+col];
            }
        }

        clear_row(VGA_HEIGHT-1);

        cursor_row=VGA_HEIGHT-1;
    }

    static void advance_cursor()
    {
        cursor_col++;

        if(cursor_col>=VGA_WIDTH)
        {
            cursor_col=0;
            cursor_row++;
        }

        if(cursor_row>=VGA_HEIGHT)
        {
            scroll_up();
        }
    }
}