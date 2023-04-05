#include "client.h"


int Client::Connect(Logger &logger) {
    int port_no = 8081;
    char serverIp[] = "127.0.0.1\0";
    struct sockaddr_in server_addr{};
    struct hostent *server;

    server = gethostbyname(serverIp);
    if (!server) {
        logger << "ERROR, no such host\n";
        return 1;
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        logger << "ERROR opening socket\n";
        return 1;
    }
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port_no);
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        logger << "ERROR connecting\n";
        return 1;
    }
    return 0;
}

int Client::GetMessage(Logger &logger) {
    bzero(buffer, sizeof(buffer));
    int n = recv(sockfd, buffer, sizeof(buffer), 0);
    if (n < 0) {
        logger << "ERROR reading from socket\n";
        return 1;
    }
    return 0;
}

int Client::SendMessage(Logger &logger) {
    int n = send(sockfd, buffer, sizeof(buffer), 0);
    if (n < 0) {
        logger << "ERROR writing to socket\n";
        return 1;
    }
    return 0;
}

void Client::TerminateConnection() {
    close(sockfd);
}
