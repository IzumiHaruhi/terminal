#ifndef TERMINAL_MENU_HPP
#define TERMINAL_MENU_HPP

#include "command.hpp"

#ifdef TERMINAL_WINDOWS
#include <conio.h>
#endif

namespace terminal
{
    std::stringstream console_stream;

    char get_console()
    {
        char input;
        if (console_stream.get(input))
        {
            return input;
        }
        console_stream.clear();
#ifdef TERMINAL_WINDOWS
        return _getch();
#else
        return std::cin.get();
#endif
    }

    void put_console(char input) { console_stream.put(input); }

    struct menu_text_t
    {
        std::string header = "options:\n";
        std::string footer = "";
        std::string before = " |";
        std::string after = "\n";
        std::string before_selected = ">|";
        std::string after_selected = "\n";
    };

    struct menu_keyboard_t
    {
        char previous = 'w';
        char next = 's';
        char enter = ' ';
        char exit = 'e';
        char command = '/';
    };

    void menu(const menu_text_t &text = {}, const std::vector<command_t> &options = {}, const menu_keyboard_t &keyboard = {})
    {
        std::size_t pointer = 0;
        while (true)
        {
            clear_screen();
            put_output(text.header);
            for (std::size_t i = 0; i < options.size(); i++)
            {
                put_output(pointer == i ? text.before_selected : text.before);
                put_output(options[i].name);
                put_output(pointer == i ? text.after_selected : text.after);
            }
            put_output(text.footer);
            char input = get_console();
            if (input == keyboard.previous && pointer)
            {
                pointer--;
            }
            if (input == keyboard.next && pointer + 1 < options.size())
            {
                pointer++;
            }
            if (input == keyboard.enter && options.size() && options[pointer].method)
            {
                options[pointer].method();
            }
            if (input == keyboard.exit)
            {
                return;
            }
            if (input == keyboard.command)
            {
#ifdef TERMINAL_WINDOWS
                put_output(input);
#endif
                std::vector<command_t> commands = default_commands;
                commands.insert(commands.end(), {{"menu:previous", std::bind(put_console, keyboard.previous)},
                                                 {"menu:next", std::bind(put_console, keyboard.next)},
                                                 {"menu:enter", std::bind(put_console, keyboard.enter)},
                                                 {"menu:exit", std::bind(put_console, keyboard.exit)}});
                command(commands);
            }
        }
    }
}

#endif