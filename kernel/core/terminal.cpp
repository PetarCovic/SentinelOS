#include <sentinel/terminal.hpp>
#include <sentinel/types.hpp>

namespace sentinel::terminal
{
    static int cursor_row;
    static int cursor_col;

    static constexpr int VGA_WIDTH = 80;
    static constexpr int VGA_HEIGHT = 25;
    static constexpr sentinel::u16 DEFAULT_COLOR = 0x0F;
    static sentinel::u16 vga_color = DEFAULT_COLOR;
    static volatile sentinel::u16* const VGA_BUFFER =
        (volatile sentinel::u16*)0xB8000;

    void initialize()
    {
        cursor_row=0;
        cursor_col=0;

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
                cursor_row=VGA_HEIGHT-1;
            }

            return;
        }
        
        int index=cursor_row*VGA_WIDTH+cursor_col;

        VGA_BUFFER[index] = (vga_color << 8) | static_cast<sentinel::u16>(c);

        cursor_col++;
        
        if(cursor_col>=VGA_WIDTH)
        {
            cursor_col=0;
            cursor_row++;

            if(cursor_row>=VGA_HEIGHT)
            {
                cursor_row=VGA_HEIGHT-1;
            }
        }        
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

        cursor_row=0;
        cursor_col=0;
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
    }
}