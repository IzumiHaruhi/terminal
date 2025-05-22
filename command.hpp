#ifndef TERMINAL_COMMAND_HPP
#define TERMINAL_COMMAND_HPP

#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if defined _WIN32 && !defined TERMINAL_PORTABLE
#define TERMINAL_WINDOWS
#endif

namespace terminal
{
    std::stringstream input_stream;

    std::string get_input()
    {
        std::string input;
        if (!(input_stream >> quoted(input)))
        {
            input_stream.clear();
            std::cin >> quoted(input);
        }
        return input;
    }

    void put_input(const std::string &input) { input_stream << quoted(input); }

    void put_output(const std::string &input, std::ostream &(*manipulator)(std::ostream &) = std::flush) { std::cout << input << manipulator; }

    void call_system() { std::system(get_input().c_str()); }

    void clear_screen()
    {
#ifdef TERMINAL_WINDOWS
        std::system("CLS");
#else
        put_output(std::string(256, '\n'));
#endif
    }

    void exit_program() { std::exit(0); }

    void output_text() { put_output(get_input(), std::endl); }

    void unknown_command() { put_output(__func__, std::endl); }

    struct command_t
    {
        std::string name;
        std::function<void()> method;
    } default_meta{"return", unknown_command};

    std::vector<command_t> default_commands{{"call", call_system}, {"clear", clear_screen}, {"exit", exit_program}, {"output", output_text}};

    void command(const std::vector<command_t> &commands = default_commands, const command_t &meta = default_meta)
    {
        while (true)
        {
        skip:
            std::string input = get_input();
            if (input == meta.name)
            {
                return;
            }
            for (std::size_t i = 0; i < commands.size(); i++)
            {
                if (input == commands[i].name)
                {
                    commands[i].method();
                    goto skip;
                }
            }
            meta.method();
        }
    }
}

#endif