#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int get_message(int& server, char* buffer, int& bufsize, bool& isExit) {
    do {
        recv(server, buffer, bufsize, 0);
        cout << buffer << " ";
        if (*buffer == '#') {
            *buffer = '*';
            isExit = true;
        }
    } while (*buffer != '*');
    return 0;
}

int send_message(int& server, char* buffer, int& bufsize, bool& isExit) {
    do {
        cin >> buffer;
        send(server, buffer, bufsize, 0);
        if (*buffer == '#') {
            send(server, buffer, bufsize, 0);
            *buffer = '*';
            isExit = true;
        }
    } while (*buffer != '*');
}