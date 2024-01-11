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
    BUF_SIZE = 1920*1080*10
};

class Network {
public:
    explicit Network(const std::vector<std::string> &IDs);

    int Connect();

//    int StartPoll();

    int GetMessage(const std::string &id, std::unique_ptr<std::string> &dest);

    int SendMessage(const std::string &id, const std::string &message);

    void TerminateConnection();

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