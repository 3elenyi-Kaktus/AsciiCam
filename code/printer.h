#pragma once

#include "vector"
#include "string"
#include "ncurses.h"


class MainMenu {
public:
    MainMenu();

    int height;
    int width;
    std::string header;
    std::vector<std::string> window;
    int num_of_options;
};

class GUI {
public:
    MainMenu menu;
};


class Terminal {
public:
    int height;
    int width;
};


Terminal InitTerminalWindow();

void TerminateTerminalWindow();

int PrintMenu(Terminal &terminal, MainMenu &menu);

void PrintFrame(Terminal& terminal, std::vector<std::vector<u_char>>& matrix);

void ClearScreen();