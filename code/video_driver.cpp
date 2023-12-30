#include "video_driver.h"

ScreenManager::ScreenManager(int height, int width) : screen(height, std::vector<wchar_t>(width + 1, L' ')),
                                                      line_is_changed(height, 0), screen_size(height, width) {
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

void ScreenManager::refreshScreen(bool save_cursor_pos) {
    int pos_y = 0;
    Coordinates old_pos{};

    std::lock_guard<std::mutex> lock(mtx);
    if (save_cursor_pos) {
        getyx(stdscr, old_pos.y, old_pos.x);
    }
    for (int indicator: line_is_changed) {
        if (indicator) {
            mvprintw(pos_y, 0, "%ls", (wchar_t *) &(screen[pos_y][0]));
        }
        ++pos_y;
    }
    if (save_cursor_pos) {
        move(old_pos.y, old_pos.x);
    }
    refresh();
    std::fill(line_is_changed.begin(), line_is_changed.end(), 0);
}
