#pragma once

#include "vector"
#include "ncurses.h"
#include "mutex"
#include "iostream"
#include "definitions.h"

class ScreenManager {
public:
    ScreenManager(int height, int width);

    void keycodesMode(int mode);

    void echoMode(int mode);

    void cursorMode(int mode);

    void moveCursor(Coordinates &coords);

    void mvCharPrint(Coordinates &coords, char c);

    void mvStringPrint(Coordinates &coords, const std::string &str);

    void mvChunkPrint(Coordinates &coords, std::vector<std::vector<u_char>> &chunk);

    void refreshScreen(bool save_cursor_pos = false);


private:
    std::vector<std::vector<char>> screen;
    std::vector<int> line_is_changed;
    Size screen_size;
    std::mutex mtx;
};
