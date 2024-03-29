#include "user_channel.h"


int UserChannel::Create(int sock_fd) {
    struct sockaddr_in init_addr{};
    socklen_t init_size = sizeof(init_addr);
    std::cerr << "wait\n";
    initiator_fd = accept(sock_fd, (struct sockaddr *) &init_addr, &init_size);
    std::cerr << "accepted\n";
    if (initiator_fd < 0) {
        logger << "ERROR on accepting initial host\n";
        std::cout << "ERROR on accepting initial host\n";
        return -1;
    }
    logger << "Initiator: " << inet_ntoa(init_addr.sin_addr) << ", " << ntohs(init_addr.sin_port) << "\n";
    std::cout << "Initiator: " << inet_ntoa(init_addr.sin_addr) << ", " << ntohs(init_addr.sin_port) << "\n";

    srand((unsigned) time(nullptr) % INT_MAX);
    int random = rand() % 10000;
    logger << "Created channel id: " << random << "\n";
    std::cout << "Created channel id: " << random << "\n";
    std::string a = "tech " + std::to_string(random);
    for (int i = 0; i < a.size(); ++i) {
        init_buffer[i] = a[i];
    }
    init_buffer[a.size()] = '\0';
    SendMessage(initiator_fd, &init_buffer[0]);

    struct sockaddr_in acc_addr{};
    socklen_t acc_size = sizeof(acc_addr);
    acceptor_fd = accept(sock_fd, (struct sockaddr *) &acc_addr, &acc_size);
    if (acceptor_fd < 0) {
        logger << "ERROR on accepting client\n";
        std::cout << "ERROR on accepting client\n";
        return -1;
    }
    logger << "Acceptor: " << inet_ntoa(acc_addr.sin_addr) << ", " << ntohs(acc_addr.sin_port) << "\n";
    std::cout << "Acceptor: " << inet_ntoa(acc_addr.sin_addr) << ", " << ntohs(acc_addr.sin_port) << "\n";;

    while (true) {
        char *end;
        GetMessage(acceptor_fd, &acc_buffer[0]);
        if (strtol(&acc_buffer[0] + 5, &end, 10) == random) {
            std::cout << "Acceptor sent valid password\n";
            logger << "Acceptor sent valid password\n";
            std::string reply = "tech 1";
            for (int i = 0; i < reply.size(); ++i) {
                acc_buffer[i] = reply[i];
            }
            acc_buffer[reply.size()] = '\0';
            SendMessage(acceptor_fd, &acc_buffer[0]);
            break;
        }
        std::cout << "Acceptor sent invalid password: \"" << &acc_buffer[0] << "\"\n";
        logger << "Acceptor sent invalid password: \"" << &acc_buffer[0] << "\"\n";
        std::string reply = "tech 0";
        for (int i = 0; i < reply.size(); ++i) {
            acc_buffer[i] = reply[i];
        }
        acc_buffer[reply.size()] = '\0';
        SendMessage(acceptor_fd, &acc_buffer[0]);
    }
    a = "tech ";
    for (int i = 0; i < a.size(); ++i) {
        init_buffer[i] = a[i];
    }
    init_buffer[a.size()] = '\0';
    SendMessage(initiator_fd, &init_buffer[0]);
    return 0;
}

int UserChannel::GetMessage(int sockfd, char *buffer) {
    bzero(buffer, BUF_SIZE);
    ssize_t n = recv(sockfd, buffer, BUF_SIZE, MSG_WAITALL);
    if (n <= 0) {
        logger << "ERROR on reading from socket\n";
        std::cout << "ERROR on reading to socket\n";
        return -1;
    } else if (n < BUF_SIZE) {
        std::cerr << "LESS THAN BUF_SIZE: " << n << "\n";
    }
//    std::cout << "Got message: " << buffer << "\n";
    return 0;
}

int UserChannel::SendMessage(int sockfd, char *buffer) {
    ssize_t n = send(sockfd, buffer, BUF_SIZE, MSG_NOSIGNAL);
    if (n <= 0) {
        logger << "ERROR on sending to socket\n";
        std::cout << "ERROR on sending to socket\n";
        return -1;
    }
//    std::cout << "Sent message: " << buffer << "\n";
    return 0;
}

void UserChannel::StartTransmitting() {
    is_active = true;

    std::thread init([&]() {
        while (is_active) {
            if (GetMessage(initiator_fd, &init_buffer[0]) < 0) {
                logger << "Initiator disconnected\n";
                std::cout << "Initiator disconnected\n";
                TerminateChannel();
                break;
            }
            if (SendMessage(acceptor_fd, &init_buffer[0]) < 0) {
                logger << "Acceptor disconnected\n";
                std::cout << "Acceptor disconnected\n";
                TerminateChannel();
                break;
            }
        }
    });

    std::thread acc([&]() {
        while (is_active) {
            if (GetMessage(acceptor_fd, &acc_buffer[0]) < 0) {
                logger << "Acceptor disconnected\n";
                std::cout << "Acceptor disconnected\n";
                TerminateChannel();
                break;
            }
            if (SendMessage(initiator_fd, &acc_buffer[0]) < 0) {
                logger << "Initiator disconnected\n";
                std::cout << "Initiator disconnected\n";
                TerminateChannel();
                break;
            }
        }
    });

    init.join();
    acc.join();
}

void UserChannel::TerminateChannel() {
    is_active = false;
    shutdown(initiator_fd, SHUT_RDWR);
    shutdown(acceptor_fd, SHUT_RDWR);
    logger << "Channel shutted down\n";
    std::cout << "Channel shutted down\n";
}

UserChannel::UserChannel() {
    init_buffer.reserve(BUF_SIZE);
    acc_buffer.reserve(BUF_SIZE);
}
