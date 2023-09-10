#include "chat.h"

// need to implement check on chat positions availability and corner cases
Chat::Chat(const Coordinates &chat_position, const Size &chat_size, ScreenManager &manager) : scr(MANUAL, 5, 10),
                                                                                  chat_pos(chat_position),
                                                                                  chat_size(chat_size),
                                                                                  messages_pos(),
                                                                                  typing_pos(),
                                                                                  current_pos(),
                                                                                  screenManager(&manager) {
    screenManager->keycodesMode(false); // enter raw mode for escape sequences
    screenManager->cursorMode(1); // show cursor

    tcgetattr(STDIN_FILENO, &orig_termios); // save canonical terminal mode and switch to raw
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
//    raw.c_cc[VMIN] = 0;
//    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    messages_pos = {chat_pos.y, chat_pos.x + 1};
    typing_pos = {chat_pos.y + chat_size.height - 3, chat_pos.x + 1};
    current_pos = typing_pos;
}

int Chat::getChar(Logger &logger) {
    screenManager->moveCursor(current_pos);
    screenManager->refreshScreen();
    logger << "waiting for char at " << current_pos.y << " " << current_pos.x << "\n";
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return -1;
    }
    if (c == '\x1b') {
        char sequence[3];
        logger << "Trying to read a control sequence\n";
        if (read(STDIN_FILENO, &sequence[0], 1) != 1) {
            return -1;
        }
        if (sequence[0] != '[') {
            logger << "Not a correct sequence\n";
            return ESCAPE;
        }
        if (read(STDIN_FILENO, &sequence[1], 1) != 1) {
            return -1;
        }
        logger << "Got sequence \"" << sequence << "\"\n";
        if (sequence[1] == 'A') {
            logger << "up\n";
            return ARROW_UP;
        }
        if (sequence[1] == 'B') {
            logger << "down\n";
            return ARROW_DOWN;
        }
        if (sequence[1] == 'C') {
            logger << "right\n";
            return ARROW_RIGHT;
        }
        if (sequence[1] == 'D') {
            logger << "left\n";
            return ARROW_LEFT;
        }
    }
    return c;
}


bool Chat::processInput(int c, Logger &logger) {
    if (c == ARROW_UP || c == ARROW_DOWN) {
        performScroll(c, logger);
        return true;
    }
    if (c > 127) {
        logger << "Got unknown character or escape sequence: " << c << "\n";
        return true;
    }
    if (iscntrl(c) && c != 127) { // (c == 10 || c == 13) for Enter keycode check
        logger << "Control letter entered, message meant to be sent\n";
        return false;
    }
    updateMessage((char) c, logger);
    return true;
}

void Chat::updateMessage(char c, Logger &logger) {
    if (c == 127) { // Delete char, if possible
        logger << "Trying to delete char from message\n";
        if (message.empty()) {
            return;
        }
        --current_pos.x;
        screenManager->mvCharPrint(current_pos, ' ');
        message.pop_back();
    } else { // Add char, if possible
        logger << "Letter entered: " << (char) c << "\n";
        screenManager->mvCharPrint(current_pos, c);
        ++current_pos.x;
        message += c;
    }
    screenManager->refreshScreen();
}

void Chat::clearMessage() {
    std::string cleaner(message.length(), ' ');
    message.clear();
    current_pos = typing_pos;
    screenManager->mvStringPrint(current_pos, cleaner);
    screenManager->refreshScreen();
}

int Chat::processMessage(Logger &logger) {
    logger << "Message entered: " << message << "\n";
    if (message == "quit") {
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
    scr.addLine(msg, logger);
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
    std::deque<std::string> visible_lines = scr.getVisibleChunk();
    Coordinates print_coords(messages_pos);
    for (std::string line: visible_lines) {
        line.append(chat_size.width - line.length(), ' ');
        screenManager->mvStringPrint(print_coords, line);
        ++print_coords.y;
    }
    screenManager->refreshScreen(true);
}

std::string &Chat::getMessage() {
    return message;
}

Chat::~Chat() {
    screenManager->cursorMode(0); // hide cursor
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // return original terminal settings
    screenManager->keycodesMode(true); // restore escape sequences mode to ncurses
}
