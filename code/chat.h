#pragma once

#include "scroll_object.h"
#include "logger.h"
#include "cstdint"
#include "unistd.h"
#include "termios.h"
#include "video_driver.h"
#include "definitions.h"

enum Keys {
    ARROW_UP = INT8_MAX + 1, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT, ESCAPE
};

class Chat {
public:
    Chat(const Coordinates& chat_position, const Size& chat_size, ScreenManager &manager);

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
    Coordinates chat_pos;
    Size chat_size;
    Coordinates messages_pos;
    Coordinates typing_pos;
    Coordinates current_pos;

    ScreenManager *screenManager;
    struct termios orig_termios{};  // for terminal settings
};
