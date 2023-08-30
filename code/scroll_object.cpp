#include "scroll_object.h"

void ScrollObject::addLine(std::string &str, Logger &logger) {
    lines.push_back(str);
    logger << "Added line \"" + str + "\" to scroll_obj\n";
    if (visible_chunk.size() < max_num_of_visible_lines) {
        visible_chunk.push_back(str);
    }
    if (behaviour == SCROLL_DOWN_ON_UPDATE) {
        scrollDown(logger);
    }
}

std::deque<std::string> &ScrollObject::getVisibleChunk() {
    return visible_chunk;
}

bool ScrollObject::scrollDown(Logger &logger) {
    if (visible_chunk.size() == lines.size()) {
        return false;
    }
    if (lines.size() - max_num_of_visible_lines == position) {
        return false;
    }
    logger << "Perform scroll-down\n";
    visible_chunk.pop_front();
    visible_chunk.push_back(lines[position + max_num_of_visible_lines]);
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
