#pragma once

#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "logger.h"
#include "vector"
#include "unordered_map"
#include "thread"
#include "deque"
#include "condition_variable"
#include "memory"

enum {
    BUF_SIZE = 32768
};

class Network {
public:
    explicit Network(const std::vector<std::string> &IDs);

    int Connect(Logger &logger);

//    int StartPoll();

    int GetMessage(const std::string &id, std::unique_ptr<std::string> &dest, Logger &logger);

    int SendMessage(const std::string &id, const std::string &message, Logger &logger);

    void TerminateConnection(Logger &logger);

    ~Network();

private:
    int sock_fd;
    std::thread poller;
    std::vector<char> send_buf;
    std::vector<char> recv_buf;
    std::unordered_map<std::string, std::deque<std::unique_ptr<std::string>>> data_packets;
    std::unordered_map<std::string, std::condition_variable> availability;
    std::unordered_map<std::string, std::mutex> locks;
};