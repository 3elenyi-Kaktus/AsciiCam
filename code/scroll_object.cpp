#include "scroll_object.h"

ScrollObject::ScrollObject() {
    scroll_behaviour = ScrollBehaviour();
    scrollbar = ScrollbarPresence();
}

ScrollObject::ScrollObject(const ScrollObjectParams& params, const Size& obj_size) : size(obj_size) {
    scroll_behaviour = params.behaviour;
    scrollbar = params.scrollbar;
    if (scrollbar == ENABLED) {
        --size.width;
    }
}

//todo add multiline support
void ScrollObject::addLine(const std::wstring &str, Logger &logger) {
    if (str.length() > size.width) {
        std::string what = "str: \"";
        what += WTOSTRING(str) + "\" length is " + std::to_string(str.length()) + ", while scr_obj.size.width is " +
                std::to_string(size.width) + "\nNot implemented: ScrollObject::addLine";
        throw std::runtime_error(what);
    }
    lines.push_back(str);
    logger << "Added line \"" + WTOSTRING(str) + "\" to scroll_obj\n";
    if (visible_chunk.size() < size.height) {
        visible_chunk.push_back(str);
    }
    if (scroll_behaviour == SCROLL_DOWN_ON_UPDATE) {
        scrollDown(logger);
    }
}

std::deque<std::wstring> &ScrollObject::getVisibleChunk() {
    for (std::wstring& line : visible_chunk) {
        line.append(size.width - line.length(), ' ');
    }
    return visible_chunk;
}

bool ScrollObject::scrollDown(Logger &logger) {
    if (visible_chunk.size() == lines.size()) {
        return false;
    }
    if (lines.size() - size.height == position) {
        return false;
    }
    logger << "Perform scroll-down\n";
    visible_chunk.pop_front();
    visible_chunk.push_back(lines[position + size.height]);
    ++position;
    return true;
}

bool ScrollObject::scrollUp(Logger &logger) {
    if (visible_chunk.size() == lines.size()) {
        return false;
    }
    if (position == 0) {
        return false;
    }
    logger << "Perform scroll-up\n";
    visible_chunk.pop_back();
    visible_chunk.push_front(lines[position - 1]);
    --position;
    return true;
}

void ScrollObject::addScrollbar() {
    visible_chunk[0] += (u_char)178;
}
