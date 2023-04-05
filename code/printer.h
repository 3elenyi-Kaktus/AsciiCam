#pragma once

#include "vector"
#include "string"
#include "ncurses.h"

class Menu {
public:
    int height;
    int width;
    std::string header;
    std::vector<std::string> window;
    int num_of_options;
};

class MainMenu : public Menu {
public:
    MainMenu();
};

class ClientServerChoice : public Menu {
public:
    ClientServerChoice();
};

class GUI {
public:
    MainMenu menu;
    ClientServerChoice CS_choice;
};


class Terminal {
public:
    Terminal();

    ~Terminal();

    int height;
    int width;
};


int PrintMenu(Terminal &terminal, Menu &menu);

void PrintFrame(Terminal &terminal, std::vector<std::vector<u_char>> &matrix);

void ClearScreen();
