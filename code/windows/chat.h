#pragma once

#include "../window.h"
#include "../scroll_object.h"
#include "../logger.h"
#include "cstdint"
#include "unistd.h"
#include "termios.h"
#include "../devices/video_driver.h"
#include "../devices/input_device.h"


class Chat : public Window {
public:
    Chat(const Coordinates& chat_position, const Size& chat_size, ScreenManager &manager);

    void drawBorders();

    bool processNewInput();

    void updateMessage(wint_t c);

    void clearMessage();

    int processMessage();

    void addMessageToHistory();

    void addMessageToHistory(std::string &msg);

    void performScroll(int64_t key);

    void updateChat();

    std::wstring &getMessage();

    void Resize(Size new_size) override;

    ~Chat();

private:
    ScrollObject scr;
    std::wstring message;
    Coordinates messages_box_pos;
    Size messages_box_size;
    Coordinates typing_box_pos;
    Size typing_box_size;
    Coordinates current_pos;

    ScreenManager *screenManager;
    struct termios orig_termios{};  // for terminal settings
};
