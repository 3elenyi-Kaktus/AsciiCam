#include "user_channel.h"


int CreatePassPoint(Logger &logger) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        logger << "ERROR on creating socket\n";
        std::cout << "ERROR on creating socket\n";
        return -1;
    }

    int val = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));

    struct sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(33333);
    if (bind(sock_fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        logger << "ERROR on binding socket\n";
        std::cout << "ERROR on binding socket\n";
        return -1;
    }

    if (listen(sock_fd, 10) < 0) {
        logger << "ERROR on listening\n";
        std::cout << "ERROR on listening\n";
        return -1;
    }

    return sock_fd;
}

int main() {
    Logger logger;

    int sock_fd = CreatePassPoint(logger);
    if (sock_fd < 0) {
        logger << "Couldn't create passpoint socket. Terminating\n";
        std::cout << "Couldn't create passpoint socket. Terminating\n";
        sleep(1);
        return 1;
    }

    UserChannel channel;
    if (channel.Create(sock_fd, logger) < 0) {
        logger << "Couldn't create user channel. Terminating\n";
        std::cout << "Couldn't create user channel. Terminating\n";
        sleep(1);
        return 1;
    }
    channel.StartTransmitting(logger);
    logger << "Ended transmitting successfully. Terminating\n";
    std::cout << "Ended transmitting successfully. Terminating\n";
    sleep(1);
    close(channel.initiator_fd);
    close(channel.acceptor_fd);
    return 0;
}