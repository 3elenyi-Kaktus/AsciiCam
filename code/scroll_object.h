#pragma once

#include <string>
#include <vector>
#include "deque"
#include "logger.h"
#include "definitions.h"

enum ScrollBehaviour {
    MANUAL,
    SCROLL_DOWN_ON_UPDATE
};

enum ScrollbarPresence {
    ENABLED,
    DISABLED
};

struct ScrollObjectParams {
    ScrollBehaviour behaviour;
    ScrollbarPresence scrollbar;
};

class ScrollObject {
public:
    ScrollObject();

    ScrollObject(const ScrollObjectParams& params, const Size& obj_size);

    void addLine(const std::wstring &str, Logger &logger);

    std::deque<std::wstring> &getVisibleChunk();

    bool scrollDown(Logger &logger);

    bool scrollUp(Logger &logger);

    void addScrollbar();

private:
    std::vector<std::wstring> lines;
    std::deque<std::wstring> visible_chunk;
    Size size{};
    int64_t position = 0;
    ScrollBehaviour scroll_behaviour;
    ScrollbarPresence scrollbar;
};
