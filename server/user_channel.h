#pragma once

#include <csignal>
#include <cstring>
#include <arpa/inet.h>
#include <thread>
#include "../code/logger.h"


enum {
    BUF_SIZE = 32768,
};

class UserChannel {
public:
    int Create(int sock_fd, Logger &logger);

    int GetMessage(int sockfd, char *buffer, Logger &logger);

    int SendMessage(int sockfd, char *buffer, Logger &logger);

    void StartTransmitting(Logger &logger);

    void TerminateChannel(Logger &logger);

    int initiator_fd;
    int acceptor_fd;
    char init_buffer[BUF_SIZE];
    char acc_buffer[BUF_SIZE];
    std::atomic_bool is_active;
};