#include <sentinel/console.hpp>
#include <sentinel/string.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/drivers/keyboard.hpp>

namespace sentinel::console
{
    static void print_prompt();
    static void redraw_current_line();

    static constexpr int COMMAND_BUFFER_SIZE=128;

    static char command_buffer[COMMAND_BUFFER_SIZE];
    static int command_length=0;

    static int cursor_index=0;
    static int prompt_start_row=0;
    static int prompt_start_col=0;

    static char completed_line[COMMAND_BUFFER_SIZE];
    static bool line_ready=false;

    void initialize()
    {
        sentinel::string::clear(command_buffer);
        sentinel::string::clear(completed_line);
        command_length=0;
        line_ready=false;

        print_prompt();
    }

    void handle_key_event(const sentinel::drivers::keyboard::KeyEvent& event)
    {  
        if(!event.pressed)
        {
            return;
        }

        char ascii=event.ascii;

        switch(event.key)
        {
            case sentinel::drivers::keyboard::KeyCode::CHARACTER: 
                if(command_length<COMMAND_BUFFER_SIZE-1)
                {
                    for(int i=command_length; i>cursor_index; i--)
                    {
                        command_buffer[i]=command_buffer[i-1];
                    }

                    command_buffer[cursor_index]=ascii;

                    command_length++;
                    cursor_index++;
                    command_buffer[command_length]='\0';

                    redraw_current_line();
                }
                break;

            case sentinel::drivers::keyboard::KeyCode::ENTER:
                sentinel::terminal::putchar('\n');
                command_buffer[command_length]='\0';

                sentinel::string::copy(completed_line, command_buffer);
                line_ready=true;
            
                sentinel::string::clear(command_buffer);
                command_length=0;
                cursor_index=0;

                command_buffer[0]='\0';

                print_prompt();
                break;

            case sentinel::drivers::keyboard::KeyCode::BACKSPACE:
                if(cursor_index>0)
                {
                    for(int i=cursor_index; command_buffer[i]!='\0'; i++)
                    {
                        command_buffer[i-1]=command_buffer[i];
                    }

                    command_length--;
                    cursor_index--;

                    command_buffer[command_length]='\0';

                    redraw_current_line();
                }

                break;

            case sentinel::drivers::keyboard::KeyCode::DELETE:
                if(cursor_index<command_length)
                {
                    for(int i=cursor_index; command_buffer[i]!='\0'; i++)
                    {
                        command_buffer[i]=command_buffer[i+1];
                    }

                    command_length--;

                    command_buffer[command_length]='\0';

                    redraw_current_line();
                }



                break;
            case sentinel::drivers::keyboard::KeyCode::LEFT_ARROW:
                if(cursor_index>0)
                {
                    cursor_index--;
                    sentinel::terminal::set_cursor(prompt_start_row, 
                        prompt_start_col+cursor_index);
                }
                break;

            case sentinel::drivers::keyboard::KeyCode::RIGHT_ARROW:
                if(cursor_index<command_length)
                {
                    cursor_index++;
                    sentinel::terminal::set_cursor(prompt_start_row, 
                        prompt_start_col+cursor_index);
                }
                break;

            case sentinel::drivers::keyboard::KeyCode::HOME:
            cursor_index=0;
                sentinel::terminal::set_cursor(prompt_start_row, prompt_start_col);
                break;

            case sentinel::drivers::keyboard::KeyCode::END:
                cursor_index=command_length;
                sentinel::terminal::set_cursor(prompt_start_row, 
                    prompt_start_col+command_length);
                break;

            default:
                return;
        }
    }

    int get_cursor_index()
    {
        return cursor_index;
    }

    int get_prompt_start_row()
    {
        return prompt_start_row;
    }

    int get_prompt_start_col()
    {
        return prompt_start_col;
    }

    void set_cursor_index(int index)
    {
        if(index>command_length)
        {
            index=command_length;
        }

        if(index<0)
        {
            index=0;
        }

        cursor_index=index;
    }

    void set_prompt_start_row(int prompt_row)
    {
        prompt_start_row=prompt_row;
    }

    void set_prompt_start_col(int prompt_col)
    {
        prompt_start_col=prompt_col;
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

    static void print_prompt()
    {
        sentinel::terminal::write("Sentinel> ");
        prompt_start_row=sentinel::terminal::get_cursor_row();
        prompt_start_col=sentinel::terminal::get_cursor_col();
        cursor_index=0;
    }

    static void redraw_current_line()
    {
        sentinel::terminal::set_cursor(prompt_start_row, prompt_start_col);
        sentinel::terminal::write(command_buffer);
        sentinel::terminal::write(" ");
        sentinel::terminal::set_cursor(prompt_start_row, 
            prompt_start_col+cursor_index);
    }
}