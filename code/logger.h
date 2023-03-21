#pragma once

#include "fstream"

class Logger {
public:
    Logger();

    void ClearLog();

    std::fstream &operator<<(const std::string &log);

    ~Logger();

private:
    std::fstream error_log;
    std::string path;
};
