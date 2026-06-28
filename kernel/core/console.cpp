#include <sentinel/console.hpp>
#include <sentinel/string.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/drivers/keyboard.hpp>

namespace sentinel::console
{
    static constexpr int COMMAND_BUFFER_SIZE=128;

    static char command_buffer[COMMAND_BUFFER_SIZE];
    static int command_length=0;

    static char completed_line[COMMAND_BUFFER_SIZE];
    static bool line_ready=false;

    void initialize()
    {
        sentinel::string::clear(command_buffer);
        sentinel::string::clear(completed_line);
        command_length=0;
        line_ready=false;

        sentinel::terminal::write("Sentinel> ");
    }

    void handle_key_event(const sentinel::drivers::keyboard::KeyEvent& event)
    {  
        if(!event.pressed)
        {
            return;
        }

        char ascii=event.ascii;

        if(ascii==0)
        {
            return;
        }

        if(ascii=='\n')
        {
            sentinel::terminal::putchar('\n');
            command_buffer[command_length]='\0';

            sentinel::string::copy(completed_line, command_buffer);
            line_ready=true;
            
            sentinel::string::clear(command_buffer);
            command_length=0;

            sentinel::terminal::write("Sentinel> ");
            return;
        }

        if(ascii=='\b')
        {
            if(command_length>0)
            {
                command_length--;
                command_buffer[command_length]='\0';

                sentinel::terminal::backspace();
            }

            return;
        }

        if(command_length<COMMAND_BUFFER_SIZE-1)
        {
            command_buffer[command_length]=ascii;
            command_length++;
            command_buffer[command_length]='\0';

            sentinel::terminal::putchar(ascii);
        }
    }

    bool has_line()
    {
        return line_ready;
    }

    bool read_line(char* out_buffer)
    {
        if(out_buffer==nullptr)
        {
            return false;
        }

        if(!line_ready)
        {
            return false;
        }

        sentinel::string::copy(out_buffer, completed_line);
        sentinel::string::clear(completed_line);
        line_ready=false;

        return true;
    }
}