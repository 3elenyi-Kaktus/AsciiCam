#include "chat.h"
#include <cmath>

std::mutex chat_mtx;
// Takes top left corner position, size and handler to videodriver
// need to implement check on chat positions availability and corner cases
Chat::Chat(const Coordinates &chat_position_, const Size &chat_size_, ScreenManager &manager_) : Window(chat_position_, chat_size_),
                                                                                                 scr(),
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

    messages_box_pos = {position.y + 1, position.x + 1}; // offset for borders
    typing_box_size = {std::max(1L, (int64_t)ceil((double)(size.height - 3) * 5 / 100)), size.width - 2};
    messages_box_size = {size.height - typing_box_size.height - 3, size.width - 2};
    typing_box_pos = {position.y + size.height - typing_box_size.height - 1, position.x + 1};
    current_pos = {typing_box_pos.y, typing_box_pos.x + 2};

    scr = ScrollObject(ScrollObjectParams(MANUAL, ENABLED_WHEN_NEEDED), messages_box_size);
    drawBorders();
}

/// Draw borders for chat including divider between chat and input field (using: ┌ ─ ┐ └ ┘ │)
void Chat::drawBorders() {
    std::wstring top = L"┌";
    top.append(size.width - top.length() - 1, L'─');
    top += L'┐';
    Coordinates coords(position.y, position.x);
    screenManager->mvStringPrint(coords, top);
    for (int64_t i = position.y + 1; i < position.y + size.height - 1; ++i) {
        coords.y = i;
        coords.x = position.x;
        screenManager->mvCharPrint(coords, L'│');
        coords.x += size.width - 1;
        screenManager->mvCharPrint(coords, L'│');
    }
    std::wstring divider(size.width - 2, L'─');
    Coordinates div_coords(typing_box_pos.y - 1, typing_box_pos.x);
    screenManager->mvStringPrint(div_coords, divider);
    screenManager->mvCharPrint(typing_box_pos, L'>');
    std::wstring bottom = L"└";
    bottom.append(size.width - bottom.length() - 1, L'─');
    bottom += L'┘';
    ++coords.y;
    coords.x = position.x;
    screenManager->mvStringPrint(coords, bottom);
}

bool Chat::processNewInput() {
    screenManager->moveCursor(current_pos);
    screenManager->refreshScreen(true);
//    logger << "waiting for char at " << current_pos.y << " " << current_pos.x << "\n";
    std::pair<KeyType, wint_t> keypress = InputManager::getKeypress();
    KeyType key_type = keypress.first;
    wint_t symbol = keypress.second;
    if (key_type == SYMBOL || key_type == DELETE) {
        updateMessage(symbol);
        return true;
    }
    if (key_type == ARROW_UP || key_type == ARROW_DOWN) {
        performScroll(key_type);
        return true;
    }
    if (key_type == ENTER) {
        logger << "Control letter entered, message meant to be sent\n";
        return false;
    }
}

void Chat::updateMessage(wint_t symb) {
    if (symb == 127) { // Delete char, if possible
//        logger << "Trying to delete char from message\n";
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
//        logger << "Letter entered: " << symb << "\n";
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

int Chat::processMessage() {
    logger << "Message entered: " << WTOSTRING(message) << "\n";
    if (message == L"quit") {
        logger << "User quit from chat\n";
        return -1;
    }
    addMessageToHistory();
    return 0;
}

void Chat::addMessageToHistory() {
    scr.addLine(message);
}

void Chat::addMessageToHistory(std::string &msg) {
    scr.addLine(STRINGTOW(msg));
}

void Chat::performScroll(int64_t key) {
    logger << "Trying to scroll\n";
    bool scroll_succeed = false;
    if (key == ARROW_UP) {
        scroll_succeed = scr.scrollUp();
    } else if (key == ARROW_DOWN) {
        scroll_succeed = scr.scrollDown();
    }
    if (scroll_succeed) {
        updateChat();
    }
}

void Chat::updateChat() {
    logger << "Updating chat\n";
    std::vector<std::wstring> visible_lines = scr.getVisibleChunk();
    Coordinates print_coords(messages_box_pos);
    for (std::wstring& line: visible_lines) {
        screenManager->mvStringPrint(print_coords, line);
        ++print_coords.y;
    }
    screenManager->refreshScreen(true);
}

std::wstring &Chat::getMessage() {
    return message;
}

//todo handle current typing position and message layout
void Chat::Resize(Size new_size) {
    logger << "\nChat resize called\n";
    if (new_size == size) {
        logger << "\nNo changes in chat size needed\n";
        return;
    }
    logger << "Change size from " << size << " to " << new_size << "\n";
    size = new_size;
    typing_box_size = {std::max(1L, (int64_t)ceil((double)(size.height - 3) * 5 / 100)), size.width - 2};
    messages_box_size = {size.height - typing_box_size.height - 3, size.width - 2};
    typing_box_pos = {position.y + size.height - typing_box_size.height - 1, position.x + 1};
//    current_pos = {typing_box_pos.y, typing_box_pos.x + 2};

    scr.Resize(messages_box_size);
    drawBorders();
    std::vector<std::wstring> visible_lines = scr.getVisibleChunk();
    Coordinates print_coords(messages_box_pos);
    for (std::wstring& line: visible_lines) {
        screenManager->mvStringPrint(print_coords, line);
        ++print_coords.y;
    }
    logger << "Resized\n";
}

Chat::~Chat() {
    screenManager->cursorMode(0); // hide cursor
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // return original terminal settings
    screenManager->keycodesMode(true); // restore escape sequences mode to ncurses
}
