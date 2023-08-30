#pragma once

#include <string>
#include <vector>
#include "deque"
#include "logger.h"

enum Behaviour {
    MANUAL, SCROLL_DOWN_ON_UPDATE
};

class ScrollObject {
public:
    ScrollObject(Behaviour behaviour, int64_t max_num_of_visible_lines, int64_t line_length) : behaviour(behaviour),
                                                                                               max_num_of_visible_lines(
                                                                                                       max_num_of_visible_lines),
                                                                                               line_length(
                                                                                                       line_length) {};

    void addLine(std::string &str, Logger &logger);

    std::deque<std::string> &getVisibleChunk();

    bool scrollDown(Logger &logger);

    bool scrollUp(Logger &logger);

private:
    std::vector<std::string> lines;
    std::deque<std::string> visible_chunk;
    int64_t position = 0;
    int64_t max_num_of_visible_lines;
    int64_t line_length;
    Behaviour behaviour;
};
