#include "send_get_message.cpp"

int server_connect(int port, int& server, int& client, sockaddr_in& server_addr) {
    socklen_t size;

    client = socket(AF_INET, SOCK_STREAM, 0);

    if (client < 0) {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }

    cout << "\n=> Socket server has been created." << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if ((bind(client, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0) {
        cout << "=> Error binding connection, the socket has already been established." << endl;
        return -1;
    }

    size = sizeof(server_addr);
    cout << "=> Looking for clients..." << endl;

    listen(client, 1);

    server = accept(client,(struct sockaddr *)&server_addr,&size);

    if (server < 0)
        cout << "=> Error on accepting." << endl;

    return 0;
}

int main() {
    int client, server;
    struct sockaddr_in server_addr;
    int port = 8080;

    server_connect(port, server, client, server_addr);

    bool isExit = false;
    int bufsize = 30000;
    char buffer[bufsize];
    int clientCount = 1;

    while (server > 0)
    {
        strcpy(buffer, "=> Server connected...\n");
        send(server, buffer, bufsize, 0);
        cout << "=> Connected with the client #" << clientCount << ", you are good to go..." << endl;
        cout << "\n=> Enter # to end the connection\n" << endl;

        cout << "Client: ";
        get_message(server, buffer, bufsize, isExit);
        do {
            cout << "\nServer: ";
            send_message(server, buffer, bufsize, isExit);
            cout << "Client: ";
            get_message(server, buffer, bufsize, isExit);
        } while (!isExit);

        cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
        close(server);
        cout << "\nGoodbye..." << endl;
        isExit = false;
        exit(1);
    }

    close(client);
    return 0;
}