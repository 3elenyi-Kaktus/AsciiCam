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

    Client(Logger &logger);

    void Connect(Logger &logger);

    void GetMessage(Logger &logger);

    void SendMessage(Logger &logger);

    void TerminateConnection();

    char buffer[32768];

private:
    int sockfd;
};