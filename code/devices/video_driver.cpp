#include "video_driver.h"
#include "../logger.h"
#include "thread"
#include "chrono"

volatile sig_atomic_t resize_signal_catched = false;


ScreenManager::ScreenManager() {
    initscr();
    getmaxyx(stdscr, screen_size.height, screen_size.width);
    screen = std::vector<std::vector<wchar_t>>(screen_size.height, std::vector<wchar_t>(screen_size.width + 1, L' '));
    line_is_changed = std::vector<int>(screen_size.height, 0);
    for (int i = 0; i < screen_size.height; ++i) {
        screen[i][screen_size.width] = L'\0';
    }
}

ScreenManager::ScreenManager(int height, int width) : screen(height, std::vector<wchar_t>(width + 1, L' ')),
                                                      line_is_changed(height, 0), screen_size(height, width) {
    initscr();
    for (int i = 0; i < screen_size.height; ++i) {
        screen[i][screen_size.width] = L'\0';
    }
}

void ScreenManager::keycodesMode(int mode) {
    keypad(stdscr, mode);
}

void ScreenManager::echoMode(int mode) {
    if (mode) {
        echo();
    } else {
        noecho();
    }
}

void ScreenManager::cursorMode(int mode) {
    curs_set(mode);
}

void ScreenManager::moveCursor(Coordinates &coords) {
    std::lock_guard<std::mutex> lock(mtx);
    move(coords.y, coords.x);
}

void ScreenManager::mvCharPrint(Coordinates &coords, wchar_t c) {
    std::lock_guard<std::mutex> lock(mtx);
    screen[coords.y][coords.x] = c;
    line_is_changed[coords.y] = 1;
}

void ScreenManager::mvStringPrint(Coordinates &coords, const std::wstring &str) {
    int length_to_copy = str.length();
    if (coords.x + str.length() > screen_size.width) {
        length_to_copy = screen_size.width - coords.x;
    }
    std::lock_guard<std::mutex> lock(mtx);
    std::copy(str.begin(), str.begin() + length_to_copy, screen[coords.y].begin() + coords.x);
    line_is_changed[coords.y] = 1;
}

void ScreenManager::mvChunkPrint(Coordinates &coords, std::vector<std::vector<u_char>> &chunk) {
    int length_to_copy = chunk[0].size();
    if (coords.x + chunk[0].size() > screen_size.width) {
        length_to_copy = screen_size.width - coords.x;
    }
    std::lock_guard<std::mutex> lock(mtx);
    for (const std::vector<u_char> &line: chunk) {
        std::copy(line.begin(), line.begin() + length_to_copy, screen[coords.y].begin() + coords.x);
        line_is_changed[coords.y] = 1;
        ++coords.y;
    }
}

int a = 0;

void ScreenManager::refreshScreen(bool save_cursor_pos) {
//    int pos_y = 0;
    Coordinates old_pos;
    ++a;
    std::lock_guard<std::mutex> lock(mtx);
//    if (a % 20 == 0) {
////        redrawwin(stdscr);
//        endwin();
//        initscr();
//    }

    if (save_cursor_pos) {
        getyx(stdscr, old_pos.y, old_pos.x);
    }
//    logger << "\n\n\nDUMP\n\n\n\n";
//    for (int indicator: line_is_changed) {
    for (int i = 0; i < screen_size.height; ++i) {
        std::wstring line(&(screen[i][0]), screen_size.width);
        if (resize_signal_catched) {
            std::cerr << "LOL 1\n";
            break;
        }
//        logger << "> " << WTOSTRING(line) << " < endswith: " << (int)screen[i].back() << "\n";
//        if (indicator) {
        mvprintw(i, 0, "%ls", line.c_str());
//        }
//        ++pos_y;
    }
    if (save_cursor_pos) {
        move(old_pos.y, old_pos.x);
    }
    refresh();
    std::fill(line_is_changed.begin(), line_is_changed.end(), 0);
}

void ScreenManager::Resize() {
    std::lock_guard<std::mutex> lock(mtx);
    Size new_size;
    getmaxyx(stdscr, new_size.height, new_size.width);
    logger << "Resize screen manager from " << screen_size << " to " << new_size << "\n";
    if (new_size.width > screen_size.width) {
        for (std::vector<wchar_t>& line : screen) {
            line.back() = L' ';
            line.resize(new_size.width + 1, L' ');
            line.back() = L'\0';
        }
    } else {
        for (std::vector<wchar_t>& line : screen) {
            line.resize(new_size.width + 1);
            line.back() = L'\0';
        }
    }
    if (new_size.height > screen_size.height) {
        screen.resize(new_size.height, std::vector<wchar_t>(new_size.width + 1, L' '));
        for (std::vector<wchar_t>& line : screen) {
            line.back() = L'\0';
        }
        line_is_changed.resize(new_size.height, 0);
    } else {
        screen.resize(new_size.height);
    }
    screen_size = new_size;
}

Size &ScreenManager::getSize() {
    return screen_size;
}

ScreenManager::~ScreenManager() {
    endwin();
}

void ScreenManager::block() {
    mtx.lock();
}

void ScreenManager::unblock() {
    mtx.unlock();
}