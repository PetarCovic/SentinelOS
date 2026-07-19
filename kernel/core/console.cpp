#include <sentinel/console.hpp>
#include <sentinel/string.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/drivers/keyboard.hpp>

namespace sentinel::console
{
    void print_prompt();
    static void redraw_current_line();
    static void save_history_command();
    static void load_history_entry(int index);
    static void load_saved_current_line();
    static void exit_history_browsing();

    static constexpr int COMMAND_BUFFER_SIZE=128;

    static char command_buffer[COMMAND_BUFFER_SIZE];
    static int command_length=0;

    static int cursor_index=0;
    static int prompt_start_row=0;
    static int prompt_start_col=0;

    static char completed_line[COMMAND_BUFFER_SIZE];
    static bool line_ready=false;

    static constexpr int HISTORY_SIZE=16;

    static char history[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
    static char current_line[COMMAND_BUFFER_SIZE];
    static int history_count=0;

    static int history_view_index=0;
    static bool browsing_history=false;

    static int last_rendered_length=0;

    void initialize()
    {
        sentinel::string::clear(command_buffer);
        sentinel::string::clear(completed_line);
        sentinel::string::clear(current_line);
        command_length=0;
        history_count=0;
        history_view_index=0;
        last_rendered_length=0;
        line_ready=false;
        browsing_history=false;

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
                exit_history_browsing();
            
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
                save_history_command();
                
                sentinel::terminal::putchar('\n');
                command_buffer[command_length]='\0';

                sentinel::string::copy(completed_line, command_buffer);
                line_ready=true;
            
                sentinel::string::clear(command_buffer);
                command_length=0;
                cursor_index=0;
                browsing_history=false;
                history_view_index=0;
                sentinel::string::clear(current_line);

                command_buffer[0]='\0';

                break;

            case sentinel::drivers::keyboard::KeyCode::BACKSPACE:
                exit_history_browsing();
            
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
                exit_history_browsing();

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

            case sentinel::drivers::keyboard::KeyCode::UP_ARROW:
                if(history_count>0 && history_view_index<history_count)
                {
                    if(!browsing_history)
                    {
                        sentinel::string::copy(current_line, command_buffer);
                        browsing_history=true;
                    }

                    history_view_index++;

                    load_history_entry(history_count-history_view_index);
                }
                break;

            case sentinel::drivers::keyboard::KeyCode::DOWN_ARROW:
                if(browsing_history)
                {
                    if(history_view_index>1)
                    {
                        history_view_index--;
                        load_history_entry(history_count-history_view_index);
                    }
                    else
                    {
                        history_view_index=0;
                        load_saved_current_line();
                    }
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

    int get_command_buffer_size()
    {
        return COMMAND_BUFFER_SIZE;
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

    void print_prompt()
    {
        sentinel::terminal::write("Sentinel> ");
        prompt_start_row=sentinel::terminal::get_cursor_row();
        prompt_start_col=sentinel::terminal::get_cursor_col();
        cursor_index=0;
        last_rendered_length=0;
    }

    static void redraw_current_line()
    {
        sentinel::terminal::set_cursor(prompt_start_row, prompt_start_col);
        sentinel::terminal::write(command_buffer);

        int extra_spaces=last_rendered_length-command_length;

        for(int i=0; i<extra_spaces; i++)
        {
            sentinel::terminal::putchar(' ');
        }

        last_rendered_length=command_length;

        sentinel::terminal::set_cursor(prompt_start_row, 
            prompt_start_col+cursor_index);
    }

    static void save_history_command()
    {
        if(sentinel::string::is_empty(command_buffer))
        {
            return;
        }

        if(history_count<HISTORY_SIZE)
        {
            sentinel::string::copy(history[history_count], command_buffer);
            history_count++;
        }
        else
        {
            for(int i=1; i<HISTORY_SIZE; i++)
            {
                sentinel::string::copy(history[i-1], history[i]);
            }

            sentinel::string::copy(history[HISTORY_SIZE - 1], command_buffer);
        }

        history_view_index=0;
        browsing_history=false;
        sentinel::string::clear(current_line);
    }

    static void load_history_entry(int index)
    {
        if(index<0 || index>=history_count)
        {
            return;
        }

        sentinel::string::copy(command_buffer, history[index]);

        command_length=sentinel::string::length(command_buffer);
        cursor_index=command_length;

        redraw_current_line();
    }

    static void load_saved_current_line()
    {
        sentinel::string::copy(command_buffer, current_line);

        sentinel::string::clear(current_line);

        command_length=sentinel::string::length(command_buffer);
        cursor_index=command_length;

        browsing_history=false;

        redraw_current_line();
    }

    static void exit_history_browsing()
    {
        if(!browsing_history)
        {
            return;
        }

        browsing_history=false;
        history_view_index=0;
        sentinel::string::clear(current_line);
    }
}