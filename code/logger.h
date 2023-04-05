#pragma once

#include "fstream"

class Logger {
public:
    Logger();

    void ClearLog();

    template<class T>
    std::fstream &operator<<(T log) {
        error_log << log;
        return error_log;
    }

    ~Logger();

private:
    std::fstream error_log;
    std::string path;
};
