#include "printer.h"


// std::string smth = "┌ ─ ┐ └ ┘ │"; for possible window fraction

void Menu::Initialize() {
    num_of_options = options.size();
    up_bottom_pad = 1;
    left_right_pad = 3; // minimum 2
    header_offset = 5;
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
            "Debug",
    };
    Initialize();
}

ClientServerChoice::ClientServerChoice() {
    header = "Do you want to create a new videochat or join existing one?";
    options = {
            "Create new",
            "Join existing",
            "Return to menu",
    };
    Initialize();
}

InputMenu::InputMenu() {
    header = "Please, enter the room id to connect to:";
    input_lines = {
            ">       ",
            "  ¯¯¯¯¯¯",
    };
    height = input_lines.size();
    width = std::max_element(input_lines.begin(), input_lines.end(),
                             [](const auto &a, const auto &b) {
                                 return a.length() < b.length();
                             })->length(); // add padding and borders width
    header_offset = 2;
}

Terminal::Terminal() {
    initscr();
    keypad(stdscr, true); // Enable system keys
    curs_set(0); // Hide cursor
    noecho(); // Disable visible input
    getmaxyx(stdscr, height, width);
}

void Terminal::UpdateSize() {
    getmaxyx(stdscr, height, width);
}

Terminal::~Terminal() {
    endwin();
}

std::pair<int, int> PrintMenu(Terminal &terminal, Menu &menu) { // need to add terminal size compatibility
    terminal.UpdateSize();
    clear();
    int pos_x = (terminal.width - menu.width) / 2;
    int pos_y = (terminal.height - menu.height) / 2 - menu.header_offset;
    mvprintw(pos_y, pos_x - (menu.header.length() - menu.width) / 2, "%s", menu.header.c_str());

    for (int i = 0; i < menu.height; ++i) {
        mvprintw(pos_y + i + menu.header_offset, pos_x, "%s", menu.print_lines[i].c_str());
    }
    int cursor_pos_y = pos_y + (menu.up_bottom_pad + 1) + menu.header_offset;
    const int cursor_pos_x = pos_x + menu.left_right_pad - 1;
    mvaddch(cursor_pos_y, cursor_pos_x, '>'); // draw initial cursor
    refresh();
    return std::make_pair(cursor_pos_y, cursor_pos_x);
}

int GetOption(std::pair<int, int> &coords, int num_of_options) {
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
            case ENTER_KEY:
                return chosen_option;
            default: // unknown
                break;
        }
    }
}

std::pair<int, int>
PrintInputMenu(Terminal &terminal, InputMenu &input_menu) { // to be reworked, if multiple input menus added
    terminal.UpdateSize();
    clear();
    int pos_x = (terminal.width - input_menu.width) / 2;
    int pos_y = (terminal.height - input_menu.height) / 2 - input_menu.header_offset;
    mvprintw(pos_y, pos_x - (input_menu.header.length() - input_menu.width) / 2, "%s", input_menu.header.c_str());
    for (int i = 0; i < input_menu.height; ++i) {
        mvprintw(pos_y + i + input_menu.header_offset, pos_x, "%s", input_menu.input_lines[i].c_str());
    }
    refresh();
    pos_y += input_menu.header_offset;  // Made for one and only input menu directly,
    pos_x += 2;                         // values may be wrong for other menus
    return std::make_pair(pos_y, pos_x);
}

std::string GetInputFromInputMenu(std::pair<int, int> &coords) { // is made only for password check. To be reworked for overall purposes
    int pos_y = coords.first;
    int pos_x = coords.second;
    curs_set(1); // Show cursor
    std::string input;
    while (true) {
        move(pos_y, pos_x);
        int a = getch();
        if (a == ENTER_KEY) { // Enter pressed
            break;
        } else if (a == KEY_BACKSPACE) { // Delete char, if possible
            if (input.empty()) {
                continue;
            }
            --pos_x;
            mvaddch(pos_y, pos_x, ' ');
            input.pop_back();
        } else { // Add char, if possible
            if (input.length() >= 6) {
                continue;
            }
            mvaddch(pos_y, pos_x, a);
            ++pos_x;
            input += (char) a;
        }
    }
    curs_set(0); // Hide cursor
    return input;
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



