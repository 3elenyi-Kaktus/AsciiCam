#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {

    int client;
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "\nError: could not create socket" << endl;
        return 1;
    }
    cout << "\n=> Socket client has been created." << endl;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error: could not connect to server" << std::endl;
        return 1;
    }
    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        cout << "=> Connection to the server port number: " << SERVER_PORT << endl;
    }
    cout << "=> Awaiting confirmation from the server..." << endl;
    recv(client, buffer, bufsize, 0);
    cout << "=> Connection confirmed.";

    cout << "\n\n=> Enter # to end the connection.\n" << endl;
    do {
        cout << "Client: ";
        do {
            cin >> buffer;
            send(client, buffer, bufsize, 0);
            if (*buffer == '#') {
                send(client, buffer, bufsize, 0);
                *buffer = '*';
                isExit = true;
            }
        } while (*buffer != 42);
        cout << "Server: ";
        do {
            recv(client, buffer, bufsize, 0);
            cout << buffer << " ";
            if (*buffer == '#') {
                *buffer = '*';
                isExit = true;
            }
        } while (*buffer != 42);
        cout << endl;
    } while (!isExit);
    cout << "\n=> Connection terminated.\n";
    close(client);
    return 0;
}