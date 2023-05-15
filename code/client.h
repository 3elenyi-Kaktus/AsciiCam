#pragma once

#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "logger.h"

enum {BUF_SIZE = 32768};

class Client {
public:
    Client() = default;

    int Connect(Logger &logger);

    int GetMessage(Logger &logger);

    int SendMessage(Logger &logger);

    void TerminateConnection(Logger &logger) const;

    char buffer[BUF_SIZE];

private:
    int sock_fd;
};