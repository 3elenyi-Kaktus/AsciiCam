#pragma once

#include "iostream"
#include "random"
#include "climits"
#include <csignal>
#include <cstring>
#include <arpa/inet.h>
#include <thread>
#include "../code/logger.h"
#include "vector"


enum {
    BUF_SIZE = 1920*1080*10,
};

class UserChannel {
public:
    UserChannel();

    int Create(int sock_fd);

    int GetMessage(int sockfd, char *buffer);

    int SendMessage(int sockfd, char *buffer);

    void StartTransmitting();

    void TerminateChannel();

    int initiator_fd;
    int acceptor_fd;
    std::vector<char> init_buffer;
    std::vector<char> acc_buffer;
    std::atomic_bool is_active;
};