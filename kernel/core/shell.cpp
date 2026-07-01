#include <sentinel/shell.hpp>
#include <sentinel/console.hpp>
#include <sentinel/string.hpp>
#include <sentinel/terminal.hpp>
#include <sentinel/panic.hpp>

namespace sentinel::shell
{
    static void execute_help();
    static void execute_clear();
    static void execute_version();
    static void execute_echo(const char* command);
    static void execute_panic_test();
    static void execute_idt_test();
    static void command_not_found();

    void execute_command(const char* command)
    {
        if(command==nullptr)
        {
            return;
        }

        if(sentinel::string::equals_ignore_case(command, "help"))
        {
            execute_help();
        }
        else if(sentinel::string::equals_ignore_case(command, "clear"))
        {
            execute_clear();
        }
        else if(sentinel::string::equals_ignore_case(command, "version"))
        {
            execute_version();
        }
        else if(sentinel::string::starts_with_ignore_case(command, "echo "))
        {
            execute_echo(command);
        }
        else if(sentinel::string::equals_ignore_case(command, "panic-test"))
        {
            execute_panic_test();
        }
        else if(sentinel::string::equals_ignore_case(command, "idt-test"))
        {
            execute_idt_test();
        }
        else
        {
            command_not_found();
        }
    }

    static void execute_help()
    {
        sentinel::terminal::writeln("HELP:         Pulls up this message with all valid commands");
        sentinel::terminal::writeln("CLEAR:        Clears the console of all previous text");
        sentinel::terminal::writeln("VERSION:      Prints out the current SentinelOS version");
        sentinel::terminal::writeln("ECHO:         Displays messages");
        sentinel::terminal::writeln("PANIC-TEST:   Performs a panic test");
        sentinel::terminal::writeln("IDT-TEST:     Performs an idt test");
    }

    static void execute_clear()
    {
        sentinel::terminal::clear();
    }

    static void execute_version()
    {
        sentinel::terminal::write("SentinelOS Version: ");
        sentinel::terminal::writeln(sentinel::terminal::get_version());
    }

    static void execute_echo(const char* command)
    {
        for(int i=5; command[i]!='\0'; i++)
        {
            sentinel::terminal::putchar(command[i]);
        }

        sentinel::terminal::putchar('\n');
    }

    static void execute_panic_test()
    {
        sentinel::panic("Performing Panic Test");
    }

    static void execute_idt_test()
    {
        sentinel::terminal::writeln("Triggering IDT test: Invalid opcode exception");

        __asm__ volatile("ud2");

        sentinel::terminal::writeln("IDT test returned");
    }

    static void command_not_found()
    {
        sentinel::terminal::writeln("Command not found");
    }
}