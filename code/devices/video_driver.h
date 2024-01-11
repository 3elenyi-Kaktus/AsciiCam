#pragma once

#include "vector"
#include "ncurses.h"
#include "mutex"
#include "iostream"
#include "../definitions.h"
#include "csignal"

extern volatile sig_atomic_t resize_signal_catched;

class ScreenManager {
public:
    ScreenManager();

    ScreenManager(int height, int width);

    void keycodesMode(int mode);

    void echoMode(int mode);

    void cursorMode(int mode);

    void moveCursor(Coordinates &coords);

    void mvCharPrint(Coordinates &coords, wchar_t c);

    void mvStringPrint(Coordinates &coords, const std::wstring &str);

    void mvChunkPrint(Coordinates &coords, std::vector<std::vector<u_char>> &chunk);

    void refreshScreen(bool save_cursor_pos = false);

    void Resize();

    void block();

    void unblock();

    Size& getSize();

    ~ScreenManager();

private:
    std::vector<std::vector<wchar_t>> screen;
    std::vector<int> line_is_changed;
    Size screen_size;
    std::mutex mtx;
};
