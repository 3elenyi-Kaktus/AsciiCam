#include "scroll_object.h"
#include "cmath"

ScrollObject::ScrollObject() : size(), position(0), scroll_behaviour(), scrollbar_behaviour() {}

ScrollObject::ScrollObject(const ScrollObjectParams &params, const Size &obj_size) : size(obj_size), position(0) {
    scroll_behaviour = params.behaviour;
    scrollbar_behaviour = params.scrollbar;
    if (scrollbar_behaviour != DISABLED) {
        --size.width;
    }
}

/// Add wstring to scroll object
void ScrollObject::addLine(const std::wstring &str) {
    uint64_t curr_pos = 0;
    while (curr_pos < str.length()) {
        uint64_t to_copy_len = std::min((size_t)size.width, str.length() - curr_pos);
        std::wstring substring = str.substr(curr_pos, to_copy_len);
        lines.push_back(substring);
        logger << "Added line \"" + WTOSTRING(substring) + "\" to scroll_obj\n";
        if (visible_chunk.size() < size.height) {
            visible_chunk.push_back(substring);
        }
        if (scroll_behaviour == SCROLL_DOWN_ON_UPDATE) {
            while (scrollDown()) {}
        }
        curr_pos += to_copy_len;
    }
}

/// Returns current visible chunk of scroll object
std::deque<std::wstring> ScrollObject::getVisibleChunk() {
    std::deque<std::wstring> visible = visible_chunk;
    for (std::wstring &line: visible) {
        line.append(size.width - line.length(), ' ');
    }
    while (visible.size() < size.height) {
        visible.emplace_back(size.width, L' ');
    }
    if (scrollbar_behaviour == DISABLED ||
        scrollbar_behaviour == ENABLED_WHEN_NEEDED && visible_chunk.size() == lines.size()) {
        return visible;
    }
    int64_t bar_size = size.height - 2;
    int64_t bar_ratio = (int64_t)std::max(1.0l, roundl(bar_size * visible_chunk.size() / (long double)lines.size()));
    int64_t bar_pos;
    if (position + visible_chunk.size() == lines.size()) {
        bar_pos = bar_size - bar_ratio;
    } else if (position == 0) {
        bar_pos = 0;
    } else {
        bar_pos = (int64_t)roundl(bar_size * position / (long double)lines.size());
        if (bar_pos + bar_ratio == bar_size) { // so that bar touches floor only if last line is visible
            --bar_pos;
        }
        if (bar_pos == 0) { // so that bar touches ceiling only if first line is visible
            ++bar_pos;
        }
    }
    std::wstring scrollbar = L"▴";
    scrollbar.append(bar_pos, L'|');
    scrollbar.append(bar_ratio, L'▓');
    scrollbar.append(bar_size - bar_pos - bar_ratio, L'|');
    scrollbar += L"▾";
    for (int i = 0; i < visible.size(); ++i) {
        visible[i] += scrollbar[i];
    }
    return visible;
}

/// Try to scroll down if possible
bool ScrollObject::scrollDown() {
    if (visible_chunk.size() == lines.size()) {
        return false;
    }
    if (position == lines.size() - size.height) {
        return false;
    }
    logger << "Perform scroll-down\n";
    visible_chunk.pop_front();
    visible_chunk.push_back(lines[position + size.height]);
    ++position;
    return true;
}

/// Try to scroll up if possible
bool ScrollObject::scrollUp() {
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
