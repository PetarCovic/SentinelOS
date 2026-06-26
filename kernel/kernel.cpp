void print(const char* msg);

//static int cursor_row = 0;
//static int cursor_col = 0;

//void putchar(char c);
//void print(const char* msg);
//void println(const char* msg);

extern "C" void kernel_main()
{
    print("SentinelOS Kernel Loaded");

    while (true)
    {
        __asm__ volatile ("hlt");
    }
}

void print(const char* msg)
{
    volatile unsigned short* vga = (unsigned short*)0xB8000;

    for (int i = 0; msg[i] != '\0'; i++)
    {
        vga[80*5+i] = (0x0F << 8) | msg[i];
    }
}

//void println(const char* msg)
//{
    //print(msg);
    //cursor_row++;
    //cursor_col = 0;
//}