#pragma once
#include "iostream"
#include "chrono"
#include "thread"
#include "ncurses.h"
#include "vector"


class MainMenu {
public:
    MainMenu();
    int height;
    int width;
    std::string header;
    std::vector<std::string> window;
    int num_of_options;
};

class Terminal {
public:
    int ROWS = 0;
    int COLS = 0;
    MainMenu menu;
};



void Setup(Terminal* terminal);
void PrintMenu(Terminal &terminal);
void PrintParams(Terminal& terminal);
void CamVideo();