#include "user_channel.h"


int UserChannel::Create(int sock_fd, Logger &logger) {
    struct sockaddr_in init_addr{};
    socklen_t init_size = sizeof(init_addr);
    initiator_fd = accept(sock_fd, (struct sockaddr *) &init_addr, &init_size);
    if (initiator_fd < 0) {
        logger << "ERROR on accepting initial host\n";
        return -1;
    }
    logger << "Initiator: " << inet_ntoa(init_addr.sin_addr) << ", " << ntohs(init_addr.sin_port) << "\n";

    struct sockaddr_in acc_addr{};
    socklen_t acc_size = sizeof(acc_addr);
    acceptor_fd = accept(sock_fd, (struct sockaddr *) &acc_addr, &acc_size);
    if (acceptor_fd < 0) {
        logger << "ERROR on accepting client\n";
        return -1;
    }
    logger << "Acceptor: " << inet_ntoa(acc_addr.sin_addr) << ", " << ntohs(acc_addr.sin_port) << "\n";
    return 0;
}

int UserChannel::GetMessage(int sockfd, char *buffer, Logger &logger) {
    bzero(buffer, BUF_SIZE);
    ssize_t n = recv(sockfd, buffer, BUF_SIZE, MSG_WAITALL);
    if (n <= 0) {
        logger << "ERROR on reading from socket\n";
        std::cout << "ERROR on reading to socket\n";
        return -1;
    }
    if (strlen(buffer) < 50) {
        logger << "Got message: " << buffer << "\n";
    }
    return 0;
}

int UserChannel::SendMessage(int sockfd, char *buffer, Logger &logger) {
    ssize_t n = send(sockfd, buffer, BUF_SIZE, MSG_NOSIGNAL);
    if (n <= 0) {
        logger << "ERROR on sending to socket\n";
        std::cout << "ERROR on sending to socket\n";
        return -1;
    }
    if (strlen(buffer) < 50) {
        logger << "Sent message: " << buffer << "\n";
    }
    return 0;
}

void UserChannel::StartTransmitting(Logger &logger) {
    is_active = true;

    std::thread init([&]() {
        while (is_active) {
            if (GetMessage(initiator_fd, init_buffer, logger) < 0) {
                logger << "Initiator disconnected\n";
                std::cout << "Initiator disconnected\n";
                TerminateChannel(logger);
                break;
            }
            if (SendMessage(acceptor_fd, init_buffer, logger) < 0) {
                logger << "Acceptor disconnected\n";
                std::cout << "Acceptor disconnected\n";
                TerminateChannel(logger);
                break;
            }
        }
    });

    std::thread acc([&]() {
        while (is_active) {
            if (GetMessage(acceptor_fd, acc_buffer, logger) < 0) {
                logger << "Acceptor disconnected\n";
                std::cout << "Acceptor disconnected\n";
                TerminateChannel(logger);
                break;
            }
            if (SendMessage(initiator_fd, acc_buffer, logger) < 0) {
                logger << "Initiator disconnected\n";
                std::cout << "Initiator disconnected\n";
                TerminateChannel(logger);
                break;
            }
        }
    });

    init.join();
    acc.join();
}

void UserChannel::TerminateChannel(Logger &logger) {
    is_active = false;
    shutdown(initiator_fd, SHUT_RDWR);
    shutdown(acceptor_fd, SHUT_RDWR);
    logger << "Channel shutted down\n";
    std::cout << "Channel shutted down\n";
}