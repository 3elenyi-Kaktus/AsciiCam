#pragma once

#include "vector"
#include "string"
#include "ncurses.h"
#include "devices/terminal.h"

#define ENTER_KEY 012 // default ncurses KEY_ENTER seems to be another button

//TODO For now, normal menus and input ones have very similar interface of creation and printing. For rework in further updates
//TODO Partially solved cases with dynamic terminal size changes

class Menu {
public:
    void Initialize();

    int up_bottom_pad;
    int left_right_pad;
    int header_offset;
    int height;
    int width;
    std::string header;
    std::vector<std::string> options;
    std::vector<std::string> print_lines;
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

class InputMenu {
public:
    InputMenu();

    int height;
    int width;
    int header_offset;
    std::string header;
    std::vector<std::string> input_lines;
};

class GUI {
public:
    MainMenu menu;
    ClientServerChoice CS_choice;
    InputMenu passcode_enter;
};


std::pair<int, int> PrintMenu(ScreenManager& screen, Menu &menu);

int GetOption(std::pair<int, int> &coords, int num_of_options);

std::pair<int, int> PrintInputMenu(ScreenManager& screen, InputMenu &input_menu);

std::string GetInputFromInputMenu(std::pair<int, int> &coords);

void ClearScreen();
