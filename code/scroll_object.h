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
    ENABLED_WHEN_NEEDED,
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

    void addLine(const std::wstring &str);

    std::deque<std::wstring> getVisibleChunk();

    bool scrollDown();

    bool scrollUp();

private:
    std::vector<std::wstring> lines;
    std::deque<std::wstring> visible_chunk;
    Size size;
    int64_t position;
    ScrollBehaviour scroll_behaviour;
    ScrollbarPresence scrollbar_behaviour;
};
