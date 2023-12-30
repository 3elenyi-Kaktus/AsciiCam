#include "input_device.h"


std::pair<KeyType, wint_t> InputManager::getKeypress() {
    wint_t symb = getChar();
    if (symb < 32 and symb != 13 and symb != 10 and symb != 27) {
        throw std::runtime_error("Unexpected control symbol (InputManager::getKeypress)\n");
    }
    if (symb == 13 || symb == 10) {
        return {ENTER, symb};
    }
    if (symb == 127) {
        return {DELETE, symb};
    }
    if (symb != '\x1b') {
        return {SYMBOL, symb};
    }

    wint_t sequence[3];
    logger << "Escape found, try interpret a control sequence\n";
    sequence[0] = getChar();
    if (sequence[0] != '[') {
        logger << "Not a correct sequence\n";
        ungetChar(sequence[0]);
        return {ESCAPE, symb};
    }

    sequence[1] = getChar();
    logger << "Got sequence \"" << sequence << "\"\n";
    switch (sequence[1]) {
        case 'A': return {ARROW_UP, 0};
        case 'B': return {ARROW_DOWN, 0};
        case 'C': return {ARROW_RIGHT, 0};
        case 'D': return {ARROW_LEFT, 0};
        default: throw std::runtime_error(
                            "Unknown escape sequence or invalid escape interpretation (InputManager::getKeypress)\n");
    }
}

wint_t InputManager::getChar() {
    wint_t symb = getwchar();
    if (symb == WEOF) {
        throw std::runtime_error("Unexpected end of stdin (InputManager::getChar)\n");
    }
    return symb;
}

void InputManager::ungetChar(wint_t c) {
    if (ungetwc(c, stdin) == WEOF) {
        throw std::runtime_error("Failure at pushing wchar back (InputManager::ungetChar)\n");
    }
}
