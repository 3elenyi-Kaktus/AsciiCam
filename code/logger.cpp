#include "logger.h"

Logger::Logger() {
    path = "./error_log.txt";
    ClearLog();
}

void Logger::ClearLog() {
    error_log.open(path, std::ofstream::out | std::ofstream::trunc);
    error_log.close();
    error_log.open(path, std::ios::app);
}

Logger::~Logger() {
    error_log.close();
}

template<class T>
std::fstream &Logger::operator<<(T log) {
    error_log << log;
    return error_log;
}

template<>
std::fstream &Logger::operator<<(int log) {
    error_log << log;
    return error_log;
}

template<>
std::fstream &Logger::operator<<(char log) {
    error_log << log;
    return error_log;
}

template<>
std::fstream &Logger::operator<<(const char* log) {
    error_log << *log;
    return error_log;
}
