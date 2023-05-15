#include "client.h"


int Client::Connect(Logger &logger) {
    char port_no[] = "33333\0";
    char ipaddr[] = "51.250.20.93\0";

    struct addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;
    struct addrinfo *result;
    int gai_error = getaddrinfo(ipaddr, port_no, &hints, &result);
    if (gai_error) {
        logger << gai_strerror(gai_error) << "\n";
        return -1;
    }
    for (struct addrinfo *a_info = result; a_info; a_info = a_info->ai_next) {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            logger << "ERROR on opening socket\nTrying again\n";
            continue;
        }
        if (connect(sock_fd, a_info->ai_addr, a_info->ai_addrlen) < 0) {
            close(sock_fd);
            sock_fd = -1;
            logger << "ERROR on connecting\nTrying again\n";
            continue;
        }
        break;
    }
    if (sock_fd < 0) {
        logger << "Couldn't connect to any of available TCP/Ip4 addresses\n";
        return -1;
    }
    logger << "Connected to server successfully\n";
    return 0;
}

int Client::GetMessage(Logger &logger) {
    bzero(buffer, BUF_SIZE);
    ssize_t n = recv(sock_fd, buffer, BUF_SIZE, MSG_WAITALL);
    if (n <= 0) {
        logger << "ERROR on reading from socket\n";
        return -1;
    }
    return 0;
}

int Client::SendMessage(Logger &logger) {
    ssize_t n = send(sock_fd, buffer, BUF_SIZE, MSG_NOSIGNAL);
    if (n <= 0) {
        logger << "ERROR on writing to socket\n";
        return -1;
    }
    return 0;
}

void Client::TerminateConnection(Logger &logger) const {
    logger << "Connection with server terminated\n";
    close(sock_fd);
}
