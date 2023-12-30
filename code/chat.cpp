#include "chat.h"
#include <cmath>

// Takes top left corner position, size and handler to videodriver
// need to implement check on chat positions availability and corner cases
Chat::Chat(const Coordinates &chat_position_, const Size &chat_size_, ScreenManager &manager_) : scr(),
                                                                                                 chat_pos(chat_position_),
                                                                                                 chat_size(chat_size_),
                                                                                                 messages_box_pos(),
                                                                                                 typing_box_pos(),
                                                                                                 current_pos(),
                                                                                                 screenManager(&manager_) {
    screenManager->keycodesMode(false); // enter raw mode for escape sequences
    screenManager->cursorMode(1); // show cursor

    tcgetattr(STDIN_FILENO, &orig_termios); // save canonical terminal mode and switch to raw
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
//    raw.c_cc[VMIN] = 0;
//    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    messages_box_pos = {chat_pos.y + 1, chat_pos.x + 1}; // offset for borders
    typing_box_size = {std::max(1L, (int64_t)ceil((double)(chat_size_.height - 3) * 5 / 100)), chat_size.width - 2};
    messages_box_size = {chat_size.height - typing_box_size.height - 3, chat_size.width - 2};
    typing_box_pos = {chat_pos.y + chat_size.height - typing_box_size.height - 1, chat_pos.x + 1};
    current_pos = {typing_box_pos.y, typing_box_pos.x + 2};

    scr = ScrollObject(ScrollObjectParams(MANUAL, DISABLED), messages_box_size);
    drawBorders();
}

void Chat::drawBorders() {
    screenManager->mvCharPrint(typing_box_pos, L'>');
    std::string smth = "┌ ─ ┐ └ ┘ │";
    std::wstring top = L"┌";
    while (top.length() < chat_size.width - 1) {
        top += L"─";
    }
    top += L"┐";
    Coordinates coords(chat_pos.y, chat_pos.x);
    screenManager->mvStringPrint(coords, top);
    for (int64_t i = chat_pos.y + 1; i < chat_size.height - 1; ++i) {
        coords.y = i;
        coords.x = chat_pos.x;
        screenManager->mvCharPrint(coords, L'│');
        coords.x += chat_size.width - 1;
        screenManager->mvCharPrint(coords, L'│');
    }
    std::wstring divider;
    while (divider.length() < chat_size.width - 2) {
        divider += L"─";
    }
    Coordinates div_coords(typing_box_pos.y - 1, typing_box_pos.x);
    screenManager->mvStringPrint(div_coords, divider);
    std::wstring bottom = L"└";
    while (bottom.length() < chat_size.width - 1) {
        bottom += L"─";
    }
    bottom += L"┘";
    ++coords.y;
    coords.x = chat_pos.x;
    screenManager->mvStringPrint(coords, bottom);
}

bool Chat::processNewInput(Logger &logger) {
    screenManager->moveCursor(current_pos);
    screenManager->refreshScreen(true);
    logger << "waiting for char at " << current_pos.y << " " << current_pos.x << "\n";
    std::pair<KeyType, wint_t> keypress = InputManager::getKeypress(logger);
    KeyType key_type = keypress.first;
    wint_t symbol = keypress.second;
    if (key_type == SYMBOL || key_type == DELETE) {
        updateMessage(symbol, logger);
        return true;
    }
    if (key_type == ARROW_UP || key_type == ARROW_DOWN) {
        performScroll(key_type, logger);
        return true;
    }
    if (key_type == ENTER) {
        logger << "Control letter entered, message meant to be sent\n";
        return false;
    }
}

void Chat::updateMessage(wint_t symb, Logger &logger) {
    if (symb == 127) { // Delete char, if possible
        logger << "Trying to delete char from message\n";
        if (message.empty()) {
            return;
        }
        --current_pos.x;
        if (current_pos.x < typing_box_pos.x) {
            --current_pos.y;
            current_pos.x = typing_box_pos.x + typing_box_size.width - 1;
        }
        screenManager->mvCharPrint(current_pos, ' ');
        message.pop_back();
    } else { // Add char, if possible
        logger << "Letter entered: " << symb << "\n";
        screenManager->mvCharPrint(current_pos, symb);
        ++current_pos.x;
        if (current_pos.x > typing_box_pos.x + typing_box_size.width - 1) {
            ++current_pos.y;
            current_pos.x = typing_box_pos.x;
        }
        message += (wchar_t)symb;
    }
    screenManager->refreshScreen();
}

void Chat::clearMessage() {
    current_pos = {typing_box_pos.y, typing_box_pos.x};
    int64_t to_clean = message.length() + 2;
    while (to_clean > 0) {
        screenManager->mvStringPrint(current_pos, std::wstring(typing_box_size.width, ' '));
        to_clean -= typing_box_size.width;
        ++current_pos.y;
    }
    current_pos = {typing_box_pos.y, typing_box_pos.x + 2};
    screenManager->mvCharPrint(typing_box_pos, L'>');
    message.clear();
    screenManager->refreshScreen();
}

int Chat::processMessage(Logger &logger) {
    logger << "Message entered: " << WTOSTRING(message) << "\n";
    if (message == L"quit") {
        logger << "User quit from chat\n";
        return -1;
    }
    addMessageToHistory(logger);
    return 0;
}

void Chat::addMessageToHistory(Logger &logger) {
    scr.addLine(message, logger);
}

void Chat::addMessageToHistory(std::string &msg, Logger &logger) {
    scr.addLine(STRINGTOW(msg), logger);
}

void Chat::performScroll(int64_t key, Logger &logger) {
    logger << "Trying to scroll\n";
    bool scroll_succeed = false;
    if (key == ARROW_UP) {
        scroll_succeed = scr.scrollUp(logger);
    } else if (key == ARROW_DOWN) {
        scroll_succeed = scr.scrollDown(logger);
    }
    if (scroll_succeed) {
        updateChat();
    }
}

void Chat::updateChat() {
    std::deque<std::wstring> visible_lines = scr.getVisibleChunk();
    Coordinates print_coords(messages_box_pos);
    for (std::wstring line: visible_lines) {
        line.append(chat_size.width - 2 - line.length(), ' ');
        screenManager->mvStringPrint(print_coords, line);
        ++print_coords.y;
    }
    screenManager->refreshScreen(true);
}

std::wstring &Chat::getMessage() {
    return message;
}

Chat::~Chat() {
    screenManager->cursorMode(0); // hide cursor
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // return original terminal settings
    screenManager->keycodesMode(true); // restore escape sequences mode to ncurses
}
