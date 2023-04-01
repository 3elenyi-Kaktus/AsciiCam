#pragma once

#include <unistd.h>
#include "cstring"
#include "sys/types.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "logger.h"


class Host {
public:
    Host(Logger &logger);

    void Connect(Logger &logger);

    void GetMessage(Logger &logger);

    void SendMessage(Logger &logger);

    void TerminateConnection();

    char buffer[32768];

private:
    int socket_fd;
    int client_fd;
};

