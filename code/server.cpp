#include "server.h"


Host::Host(Logger &logger) {
    Connect(logger);
}

void Host::Connect(Logger &logger) {
    int port_no = 8081;
    struct sockaddr_in server_addr{}, client_addr{};

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        //logger << "ERROR opening socket\n";
        return;
    }
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_no);
    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        //logger << "ERROR on binding\n";
        return;
    }

    listen(socket_fd, 5);

    socklen_t client_len = sizeof(client_addr);
    client_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_len);
    if (client_fd < 0) {
        //logger << "ERROR on accept\n";
    }
}

void Host::GetMessage(Logger &logger) {
    bzero(buffer, sizeof(buffer));
    int n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n < 0) {
        //logger << "ERROR on receiving\n";
    }
}

void Host::SendMessage(Logger &logger) {
    int n = send(client_fd, buffer, sizeof(buffer), 0);
    if (n < 0) {
        //logger << "ERROR on sending\n";
    }
}

void Host::TerminateConnection() {
    close(client_fd);
    close(socket_fd);
}
