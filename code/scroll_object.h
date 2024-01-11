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

using Line = std::pair<uint64_t, std::wstring>;

class ScrollObject {
public:
    ScrollObject();

    ScrollObject(const ScrollObjectParams& params, const Size& obj_size);

    void addLine(const std::wstring &str);

    std::vector<std::wstring> getVisibleChunk();

    bool scrollDown();

    bool scrollUp();

    void Resize(Size new_size);

private:
    std::vector<Line> lines;
    Size size;
    uint64_t position;
    ScrollBehaviour scroll_behaviour;
    ScrollbarPresence scrollbar_behaviour;
    uint64_t id;
};
