#include "printer.h"


// std::string smth = "┌ ─ ┐ └ ┘ │"; for possible window fraction

void Menu::Initialize() {
    num_of_options = options.size();
    up_bottom_pad = 1;
    left_right_pad = 3;
    width = std::max_element(options.begin(), options.end(),
                             [](const auto &a, const auto &b) {
                                 return a.length() < b.length();
                             })->length() + left_right_pad * 2 + 2; // add padding and borders width

    std::string border_line;
    while (border_line.length() < width) {
        border_line.append("#");
    }
    print_lines.push_back(border_line); // create and add borderline

    std::string padding_line;
    padding_line.append("#");
    while (padding_line.length() < width - 1) {
        padding_line.append(" ");
    }
    padding_line.append("#");
    for (int i = 0; i < up_bottom_pad; ++i) {
        print_lines.push_back(padding_line);  // create and add padding lines
    }

    for (int i = 0; i < num_of_options; ++i) { // create and add option lines
        std::string line;
        line.append("#");
        int padding = (width - options[i].length() - 2) / 2;
        for (int j = 0; j < padding; ++j) {
            line.append(" ");
        }
        line.append(options[i]);
        while (line.length() < width - 1) {
            line.append(" ");
        }
        line.append("#");
        print_lines.push_back(line);
    }

    for (int i = 0; i < up_bottom_pad; ++i) {
        print_lines.push_back(padding_line); // add padding lines
    }

    print_lines.push_back(border_line);  // add borderline

    height = print_lines.size();
}

MainMenu::MainMenu() {
    header = "Hello world, user and CAOS enjoyer!";
    options = {
            "Enter AsciiCam",
            "some chat",
            "Parameters",
            "Exit",
    };
    Initialize();
}

ClientServerChoice::ClientServerChoice() {
    header = "Do you want to create a new videochat or join existing one?";
    options = {
            "Create new",
            "Join existing",
    };
    Initialize();
}

InputMenu::InputMenu() {
    header = "Please, enter the room id to connect to:";
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

std::pair<int, int> PrintMenu(Terminal &terminal, Menu &menu) { // need to add terminal size compatibility
    clear();
    int pos_x = (terminal.width - menu.width) / 2;
    int pos_y = (terminal.height - menu.height) / 2 - 5; // 5 for header offset from menu block
    mvprintw(pos_y, pos_x - (menu.header.length() - menu.width) / 2, "%s", menu.header.c_str());

    for (int i = 0; i < menu.height; ++i) {
        mvprintw(pos_y + i + 5, pos_x, "%s", menu.print_lines[i].c_str()); // 5 for header offset from menu block
    }
    int cursor_pos_y = pos_y + (menu.up_bottom_pad + 1) + 5; // 5 for header offset from menu block
    const int cursor_pos_x = pos_x + menu.left_right_pad;
    mvaddch(cursor_pos_y, cursor_pos_x, '>'); // draw initial cursor
    refresh();
    return std::make_pair(cursor_pos_y, cursor_pos_x);
}

int GetOption(std::pair<int, int>& coords, int num_of_options) {
    int cursor_pos_y = coords.first;
    const int cursor_pos_x = coords.second;
    int chosen_option = 1;
    while (true) {
        int a = getch();
        switch (a) {
            case KEY_UP:
                if (chosen_option > 1) { // move up, if possible
                    --chosen_option;
                    mvaddch(cursor_pos_y, cursor_pos_x, ' '); // clear old cursor
                    --cursor_pos_y;
                    mvaddch(cursor_pos_y, cursor_pos_x, '>'); // draw new cursor
                }
                break;
            case KEY_DOWN:
                if (chosen_option < num_of_options) { // move down, if possible
                    ++chosen_option;
                    mvaddch(cursor_pos_y, cursor_pos_x, ' '); // clear old cursor
                    ++cursor_pos_y;
                    mvaddch(cursor_pos_y, cursor_pos_x, '>'); // draw new cursor
                }
                break;
            case 012: // enter pressed
                return chosen_option;
            default: // unknown
                break;
        }
    }
}

std::string PrintInputMenu(Terminal &terminal, InputMenu &input_menu) {
    clear();
    int pos_x = terminal.width / 2;
    int pos_y = (terminal.height - 10) / 2; // 5 for header offset from menu block
    mvprintw(pos_y, pos_x - input_menu.header.length() / 2, "%s", input_menu.header.c_str());
    mvaddch(pos_y + 2, pos_x, '>');
    refresh();
    char input[100];
    echo();
    getstr(input);
    noecho();
    std::string in(input);
    return in;
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



