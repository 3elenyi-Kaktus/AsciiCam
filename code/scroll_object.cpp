#include "scroll_object.h"
#include "cmath"

ScrollObject::ScrollObject() : size(), position(0), scroll_behaviour(), scrollbar_behaviour(), id(0) {}

ScrollObject::ScrollObject(const ScrollObjectParams &params, const Size &obj_size) : size(obj_size), position(0),
                                                                                     id(0) {
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
        lines.emplace_back(id, substring);
//        logger << "Added line \"" << substring << "\" to scroll_obj\n";
        curr_pos += to_copy_len;
    }
    if (scroll_behaviour == SCROLL_DOWN_ON_UPDATE) {
        while (scrollDown()) {}
    }
    ++id;
}

/// Returns current visible chunk of scroll object with scrollbar, if conditions are met
std::vector<std::wstring> ScrollObject::getVisibleChunk() {
    logger << "Getting visible chunk\n";
    logger << "Size: " << size << "\n";
    std::vector<std::wstring> visible;
    uint64_t visible_size = std::min(lines.size() - position, (uint64_t) size.height);
    for (uint64_t i = position; i < position + visible_size; ++i) {
        std::wstring line = lines[i].second;
        logger << "Line bef: " << line.length() << "\n";
        line.resize(size.width, ' ');
        logger << "After: " << line.length() << "\n";
        visible.push_back(line);
    }
    logger << "pure visible size: " << visible.size() << "\n";
    visible.resize(size.height, std::wstring(size.width, L' '));
    logger << "filled with spaces visible size: " << visible.size() << "\n";
    if (scrollbar_behaviour == DISABLED ||
        scrollbar_behaviour == ENABLED_WHEN_NEEDED && lines.size() <= size.height) {
        return visible;
    }
    int64_t bar_size = size.height - 2;
    int64_t bar_ratio = (int64_t) std::max(1.0l, roundl(bar_size * visible_size / (long double) lines.size()));
    int64_t bar_pos;
    if (position + visible_size == lines.size()) {
        bar_pos = bar_size - bar_ratio;
    } else if (position == 0) {
        bar_pos = 0;
    } else {
        bar_pos = (int64_t) roundl(bar_size * position / (long double) lines.size());
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
    if (lines.size() <= size.height ||
        position == lines.size() - size.height) {
        return false;
    }
    logger << "Perform scroll-down\n";
    ++position;
    return true;
}

/// Try to scroll up if possible
bool ScrollObject::scrollUp() {
    if (lines.size() <= size.height ||
        position == 0) {
        return false;
    }
    logger << "Perform scroll-up\n";
    --position;
    return true;
}

/// Resizes scroll object to given SIZE\n\n
/// Position tends to be preserved as in previous state, but with only guarantee that it will point to the start of line with same id
void ScrollObject::Resize(Size new_size) {
    logger << "Resize on ScrollObject called\n";
    if (size == new_size) {
        logger << "Size is same, as already possessed\n";
        return;
    }
    logger << "Resizing from: " << size << " to " << new_size << "\n";
    id = 0;
    size = new_size;
    if (scrollbar_behaviour != DISABLED) {
        --size.width;
    }
    uint64_t curr_visible_pos_id = 0;
    bool needs_position_saving = false;
    if (!lines.empty()) {
        curr_visible_pos_id = lines[position].first;
        needs_position_saving = true;
    }
    std::vector<Line> new_lines = std::move(lines);
    lines.clear();
    uint64_t curr_id = 0;
    std::wstring curr_line;
    for (const Line &line: new_lines) {
//        logger << "Prepare line n." << line.first << ": " << line.second << "\n";
        if (line.first == curr_id) {
            if (needs_position_saving && line.first == curr_visible_pos_id) {
                logger << "New position set to: " << lines.size() << "\n";
                curr_visible_pos_id = lines.size();
                needs_position_saving = false;
            }
            curr_line += line.second;
        } else {
            this->addLine(curr_line);
            curr_id = line.first;
            curr_line = line.second;
        }
    }
    this->addLine(curr_line);
    logger << "Position changed from " << position << " to " << curr_visible_pos_id << "\n";
    position = curr_visible_pos_id;
}
