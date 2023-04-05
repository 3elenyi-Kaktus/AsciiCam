#pragma once

#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "logger.h"


class Client {
public:

    int Connect(Logger &logger);

    int GetMessage(Logger &logger);

    int SendMessage(Logger &logger);

    void TerminateConnection();

    char buffer[32768];

private:
    int sockfd;
};