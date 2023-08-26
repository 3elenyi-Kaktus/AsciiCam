#include "chat.h"

Chat::Chat(int pos_y, int pos_x, int width, int height) : scr(MANUAL, 5, 10), typing_pos({pos_x, pos_y}), chat_size({width, height}) {
    keypad(stdscr, false); // enter raw mode for escape sequences
    curs_set(1); // show cursor

    tcgetattr(STDIN_FILENO, &orig_termios); // save canonical terminal mode and switch to raw
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);

//    raw.c_cc[VMIN] = 0;
//    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int Chat::getChar(Logger &logger) {
    move(typing_pos.y, typing_pos.x);
    refresh();
    logger << "waiting for char at " << typing_pos.y << " " << typing_pos.x << "\n";
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
        --typing_pos.x;
        mvaddch(typing_pos.y, typing_pos.x, ' ');
        message.pop_back();
    } else { // Add char, if possible
        logger << "Letter entered: " << (char) c << "\n";
        mvaddch(typing_pos.y, typing_pos.x, c);
        ++typing_pos.x;
        message += c;
    }
    refresh();
}

void Chat::clearMessage() {
    message.clear();
    while (typing_pos.x >= 0) {
        --typing_pos.x;
        mvaddch(typing_pos.y, typing_pos.x, ' ');
    }
    typing_pos.x = 0;
    refresh();
}

int Chat::processMessage(Logger &logger) {
    logger << "Message entered: " << message << "\n";
    if (message == "quit") {
        return -1;
    }
    return 0;
}

int Chat::sendMessage(Logger &logger) {
    scr.addLine(message, logger);
    clearMessage();
    return 0;
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
    int pos_y = 0;
    int pos_x = 0;
    for (const std::string &line: visible_lines) {
        mvprintw(pos_y, pos_x, "%-*s", chat_size.width, line.c_str());
        getyx(stdscr, pos_y, pos_x);
        pos_x = 0;
    }
    move(typing_pos.y, typing_pos.x);
    refresh();
}

std::string &Chat::getMessage() {
    return message;
}

Chat::~Chat() {
    curs_set(0); // hide cursor
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // return original terminal settings
    keypad(stdscr, true); // restore escape sequences mode to ncurses
}


