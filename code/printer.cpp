#include "printer.h"
#include "webcam_capture.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "ascii_converter.h"

std::string smth = "┌ ─ ┐ └ ┘ │";

MainMenu::MainMenu() {
    header = "Some terminal heading";
    window = {
            "###################",
            "#                 #",
            "#    enter cam    #",
            "#    some dest    #",
            "#    and params   #",
            "#      exit       #",
            "#                 #",
            "###################"
    };
    height = window.size();
    width = window[0].size();
    num_of_options = 4;
}

void Setup(Terminal *terminal) {
    getmaxyx(stdscr, terminal->ROWS, terminal->COLS);
    mvwprintw(stdscr, 7, (terminal->COLS - 30) / 2, "%s", "Hello world, user and ACOS enjoyer!\n");
    refresh();
}

void PrintMenu(Terminal &terminal) {
    MainMenu menu = terminal.menu;

    keypad(stdscr, true);
    curs_set(0); //"Убиваем" курсор
    mvprintw((terminal.ROWS - menu.height) / 2 - 1, (terminal.COLS - menu.header.length()) / 2, "%s",
             menu.header.c_str());
    for (int i = 0; i < menu.height; ++i) {
        mvprintw((terminal.ROWS - menu.height) / 2 + i, (terminal.COLS - menu.width) / 2, "%s", menu.window[i].c_str());
    }
    //mvprintw(terminal.ROWS - 1, 0, "The number of rows - %d and columns - %d\n", terminal.ROWS, terminal.COLS);
    int chosen_option = 1;
    mvaddch((terminal.ROWS - menu.height) / 2 + 2, (terminal.COLS - menu.width) / 2 + 3, '>');
    refresh();

    while (true) {
        int a = getch();
        //mvprintw(0, 0, "choice: %d   ", a);
        switch (a) {
            case KEY_UP:
                if (chosen_option > 1) // move up, if possible
                    --chosen_option;
                break;
            case KEY_DOWN:
                if (chosen_option < menu.num_of_options) // move down, if possible
                    ++chosen_option;
                break;
            case 012: // enter pressed
                if (chosen_option == 1) {
                    CamVideo();
                } else if (chosen_option == 2) {

                } else if (chosen_option == 3) {
                    PrintParams(terminal);
                } else if (chosen_option == 4) {
                    endwin();
                    exit(0);
                }
                break;
            default:
                break;
        }
        for (int i = 0; i < menu.height; i++) {
            mvprintw((terminal.ROWS - menu.height) / 2 + i, (terminal.COLS - menu.width) / 2, "%s",
                     menu.window[i].c_str());
            if (i - 1 == chosen_option) {
                mvaddch((terminal.ROWS - menu.height) / 2 + i, (terminal.COLS - menu.width) / 2 + 3, '>');
            }
        }
    }
}

void CamVideo() {
    while (true) {
        if (cv::waitKey(10) >= 0) {
            break;
        }
        cv::Mat frame = GetFrame();
        cv::resize(frame, frame,cv::Size(),2, 2,cv::INTER_AREA);
        std::vector<std::vector<u_char>> ascii_matrix = ConvertFrameToASCII(frame);
        for (size_t i = 0; i < ascii_matrix.size(); ++i) {
            for (size_t j = 0; j < ascii_matrix[0].size(); ++j) {
                mvprintw(i, j, "%c", ascii_matrix[i][j]);
            }
        }
        refresh();
    }
}

void PrintParams(Terminal &terminal) {

}
