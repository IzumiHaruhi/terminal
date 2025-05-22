// #define TERMINAL_PORTABLE

#include "menu.hpp"

int main(int argc, const char **argv)
{
    std::system("chcp 65001");
    terminal::menu({"选项：\n"},
                   {{"alpha", []
                     { terminal::menu({}, {{"a"}, {"b"}}); }},
                    {"beta"}});
    return 0;
}