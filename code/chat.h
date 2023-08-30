#pragma once

#include "scroll_object.h"
#include "logger.h"
#include "ncurses.h"
#include "cstdint"
#include "unistd.h"
#include "termios.h"

enum Keys {
    ARROW_UP = INT8_MAX + 1, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, ESCAPE
};

struct Coordinates {
    int64_t x;
    int64_t y;
};

struct Size {
    int width;
    int height;
};

class Chat {
public:
    Chat(int pos_y, int pos_x, int width, int height);

    int getChar(Logger &logger);

    bool processInput(int c, Logger &logger);

    void updateMessage(char c, Logger &logger);

    void clearMessage();

    int processMessage(Logger &logger);

    void addMessageToHistory(Logger &logger);

    void addMessageToHistory(std::string &msg, Logger &logger);

    void performScroll(int64_t key, Logger &logger);

    void updateChat();

    std::string &getMessage();

    ~Chat();

private:
    ScrollObject scr;
    std::string message;
    Coordinates typing_pos;
    Size chat_size;

    struct termios orig_termios{};  // for terminal settings
};
