#include "printer.h"


// std::string smth = "┌ ─ ┐ └ ┘ │"; for possible window fraction

MainMenu::MainMenu() {
    header = "Hello world, user and ACOS enjoyer!";
    window = {
            "########################",
            "#                      #",
            "#    Enter Asciicam    #",
            "#      some  chat      #",
            "#      Parameters      #",
            "#         Exit         #",
            "#                      #",
            "########################"
    };
    height = window.size();
    width = window[0].size();
    num_of_options = 4;
}

ClientServerChoice::ClientServerChoice() {
    header = "Do you want to create a new videochat or join existing one?";
    window = {
            "########################",
            "#                      #",
            "#      Create new      #",
            "#     Join existing    #",
            "#                      #",
            "########################"
    };
    height = window.size();
    width = window[0].size();
    num_of_options = 2;
}

Terminal::Terminal() {
    initscr();
    keypad(stdscr, true); // Enable system keys
    curs_set(0); // Hide cursor
    noecho(); // Disable visible input
    getmaxyx(stdscr, height, width);
}

Terminal::~Terminal() {
    endwin();
}

int PrintMenu(Terminal &terminal, Menu &menu) {
    clear();
    int pos_x = (terminal.width - menu.width) / 2;
    int pos_y = (terminal.height - menu.height - 10) / 2; // 5 for header offset from menu block
    mvprintw(pos_y, pos_x - (menu.header.length() - menu.width) / 2, "%s", menu.header.c_str());
    for (int i = 0; i < menu.height; ++i) {
        mvprintw(pos_y + i + 5, pos_x, "%s", menu.window[i].c_str()); // 5 for header offset from menu block
    }
    mvaddch(pos_y + 2 + 5, pos_x + 3, '>');
    refresh();


    int chosen_option = 1;
    while (true) {
        int a = getch();
        switch (a) {
            case KEY_UP:
                if (chosen_option > 1) { // move up, if possible
                    --chosen_option;
                    mvaddch(pos_y + (chosen_option + 2) + 5, pos_x + 3, ' '); // clear old cursor
                    mvaddch(pos_y + (chosen_option + 1) + 5, pos_x + 3, '>'); // 5 for header offset from menu block
                }
                break;
            case KEY_DOWN:
                if (chosen_option < menu.num_of_options) { // move down, if possible
                    ++chosen_option;
                    mvaddch(pos_y + chosen_option + 5, pos_x + 3, ' '); // clear old cursor
                    mvaddch(pos_y + (chosen_option + 1) + 5, pos_x + 3, '>'); // 5 for header offset from menu block
                }
                break;
            case 012: // enter pressed
                return chosen_option;
            default: // unknown
                break;
        }
    }
}

void PrintFrame(Terminal &terminal, std::vector<std::vector<u_char>> &matrix) {
    int pos_x = (terminal.width - matrix[0].size()) / 2;
    int pos_y = (terminal.height - matrix.size()) / 2;
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[0].size(); ++j) {
            mvprintw(pos_y + i, pos_x + j, "%c", matrix[i][j]);
        }
    }
    refresh();
}

void ClearScreen() {
    clear();
    refresh();
}
