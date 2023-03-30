#include "send_get_message.cpp"

int client_connect(int& client, int port, const char* ip, char* buffer, int& bufsize) {

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "\nError: could not create socket" << endl;
        return 1;
    }
    cout << "\n=> Socket client has been created." << endl;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error: could not connect to server" << std::endl;
        return 1;
    }
    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
        cout << "=> Connection to the server port number: " << port << endl;
    }
    cout << "=> Awaiting confirmation from the server..." << endl;
    recv(client, buffer, bufsize, 0);
    cout << "=> Connection confirmed.";
}

int main() {
    const char* ip = "127.0.0.1";
    int port = 8080;
    int client;
    int bufsize = 30000;
    char buffer[bufsize];

    client_connect(client, port, ip, buffer, bufsize);

    bool isExit = false;
    cout << "\n\n=> Enter # to end the connection.\n" << endl;
    do {
        cout << "Client: ";
        send_message(client, buffer, bufsize, isExit);
        cout << "Server: ";
        get_message(client, buffer, bufsize, isExit);
        cout << endl;
    } while (!isExit);
    cout << "\n=> Connection terminated.\n";
    close(client);
    return 0;
}