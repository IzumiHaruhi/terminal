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

    int to_number(const std::string &input)
    try
    {
        return std::stoi(input);
    }
    catch (...)
    {
        return 0;
    }

    std::map<std::string, std::string> command_variables;

    void set_variable()
    {
        std::string input = get_input();
        command_variables[input] = get_input();
    }

    void get_variable() { put_input(std::quoted(command_variables[get_input()])); }

    void invoke_variable() { put_input<std::endl>(command_variables[get_input()]); }

    void delete_variable() { command_variables.erase(get_input()); }

    void get_all()
    {
        std::ostringstream result;
        for (const auto &[key, value] : command_variables)
        {
            result << std::quoted(key) << " " << std::quoted(value) << std::endl;
        }
        put_input(std::quoted(result.str().substr(0, result.str().size() - 1)));
    }

    void get_time()
    {
        std::time_t value = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        put_input(std::put_time(std::localtime(&value), "\"%F %T\""));
    }

    void get_quote()
    {
        std::ostringstream result;
        result << std::quoted(get_input());
        put_input(std::quoted(result.str()));
    }

    void get_length() { put_input(std::quoted(std::to_string(get_input().size()))); }

    void get_concatenate()
    {
        std::string input = get_input();
        put_input(std::quoted(input + get_input()));
    }

    void get_repeat()
    {
        std::string input = get_input(), result;
        for (int i = to_number(get_input()); i--;)
        {
            result += input;
        }
        put_input(std::quoted(result));
    }

    void get_condition() { put_input(std::quoted(std::vector{get_input(), get_input()}[!to_number(get_input())])); }

    void get_substring()
    {
        std::string input = get_input();
        std::size_t value = to_number(get_input());
        put_input(std::quoted(input.substr(std::min(value, input.size()), to_number(get_input()))));
    }

    void get_calculate()
    {
        int value = to_number(get_input());
        std::string input = get_input();
#define _(command_operator, ...) input == #command_operator ? put_input(std::quoted(std::to_string(value __VA_ARGS__ command_operator to_number(get_input()))))
        _(+) : _(-) : _(*) : _(/) : _(%) : _(&) : _(|) : _(^) : _(<<) : _(>>) : _(&&) : _(||) : _(==) : _(!=) : _(<) : _(>) : _(<=) : _(>=) : _(~, =) : _(!, =) : put_input(std::quoted(get_input()));
#undef _
    }

    void execute_text() { put_input<std::endl>(get_input()); }

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

    void unknown_command() { put_output<std::endl>(std::quoted(__func__)); }

    struct command_t
    {
        std::string name;
        std::function<void()> method;
    } default_meta{"return", unknown_command};

    std::vector<command_t> default_commands{{"set", set_variable},
                                            {"get", get_variable},
                                            {"invoke", invoke_variable},
                                            {"delete", delete_variable},
                                            {"all", get_all},
                                            {"time", get_time},
                                            {"quote", get_quote},
                                            {"length", get_length},
                                            {"concatenate", get_concatenate},
                                            {"repeat", get_repeat},
                                            {"condition", get_condition},
                                            {"substring", get_substring},
                                            {"calculate", get_calculate},
                                            {"execute", execute_text},
                                            {"output", output_text},
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