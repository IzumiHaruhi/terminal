#ifndef TERMINAL_COMMAND_HPP
#define TERMINAL_COMMAND_HPP

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
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
        if (!(input_stream >> std::quoted(input)))
        {
            input_stream.clear();
            std::cin >> std::quoted(input);
        }
        return input;
    }

    template <std::ostream &(*manipulator)(std::ostream &) = std::flush, typename... args_t>
    void put_input(args_t &&...args) { (input_stream << ... << args) << manipulator; }

    template <std::ostream &(*manipulator)(std::ostream &) = std::flush, typename... args_t>
    void put_output(args_t &&...args) { (std::cout << ... << args) << manipulator; }

    template <std::ostream &(*manipulator)(std::ostream &) = std::flush, typename... args_t>
    void put_log(args_t &&...args) { (std::ofstream("log.txt", std::ios_base::app) << ... << args) << manipulator; }

    char to_lowercase(char input) { return std::tolower(static_cast<unsigned char>(input)); }

    std::map<std::string, std::string> command_variables;

    void set_variable()
    {
        std::string input = get_input();
        command_variables[input] = get_input();
    }

    void get_variable() { put_input(std::quoted(command_variables[get_input()])); }

    void execute_variable() { put_input<std::endl>(command_variables[get_input()]); }

    void delete_variable() { command_variables.erase(get_input()); }

    void get_all()
    {
        std::ostringstream text_stream;
        for (const auto &[key, value] : command_variables)
        {
            text_stream << std::quoted(key) << " " << std::quoted(value) << std::endl;
        }
        put_input(std::quoted(text_stream.str().substr(0, text_stream.str().size() - 1)));
    }

    void get_time()
    {
        std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        put_input(std::put_time(std::localtime(&current_time), "\"%F %T\""));
    }

    void get_quote()
    {
        std::ostringstream text_stream;
        text_stream << std::quoted(get_input());
        put_input(std::quoted(text_stream.str()));
    }

    void get_concatenate() {}
    // concat alpha beta
    // alphabeta

    void get_calculate() {}
    // calc 1 + 2
    // 3

    void input_text() { put_input<std::endl>(get_input()); }

    void output_text() { put_output<std::endl>(get_input()); }

    void log_text() { put_log<std::endl>(get_input()); }

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

    void unknown_command() { put_output<std::endl>(__func__); }

    struct command_t
    {
        std::string name;
        std::function<void()> method;
    } default_meta{"return", unknown_command};

    std::vector<command_t> default_commands{{"set", set_variable},
                                            {"get", get_variable},
                                            {"exec", execute_variable},
                                            {"del", delete_variable},
                                            {"all", get_all},
                                            {"time", get_time},
                                            {"quote", get_quote},
                                            {"concat", get_concatenate},
                                            {"calc", get_calculate},
                                            {"in", input_text},
                                            {"out", output_text},
                                            {"log", log_text},
                                            {"call", call_system},
                                            {"clear", clear_screen},
                                            {"exit", exit_program}};

    void command(const std::vector<command_t> &commands = default_commands, const command_t &meta = default_meta)
    {
        while (true)
        {
        skip:
            std::string input = get_input();
            std::transform(input.begin(), input.end(), input.begin(), to_lowercase);
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