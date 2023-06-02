#pragma once

#include "fstream"
#include "mutex"

class Logger {
public:
    Logger();

    void ClearLog();

    template<class T>
    std::fstream &operator<<(T log) {
        std::lock_guard<std::mutex> lock_(writer);
        error_log << log << std::flush;
        return error_log;
    }

    ~Logger();

private:
    std::fstream error_log;
    std::string path;
    std::mutex writer;
};
