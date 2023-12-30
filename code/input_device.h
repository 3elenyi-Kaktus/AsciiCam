#include "logger.h"
#include "unistd.h"

enum KeyType {
    SYMBOL,

    ENTER,
    ESCAPE,
    DELETE,

    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
};


class InputManager {
public:
    InputManager() = default;

    static std::pair<KeyType, wint_t> getKeypress();

private:
    static wint_t getChar();

    static void ungetChar(wint_t c);
};