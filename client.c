#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in server;
    int http_socket;
    int port = 1337;

    http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1) {
        fprintf(stderr, "Socket Error\n");
        return 1;
    }

    printf("WE MADE A SOCKET CONNECTION: %d\n", http_socket);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(http_socket, (struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "Could not connect to server\n");
        return 1;
    }

    printf("WE CONNECTED TO SERVER\n");

    char msg[] = "Hi there!";
    if (send(http_socket, msg, sizeof(msg), 0) == -1) {
        fprintf(stderr, "Could not send message to server");
    }

    return 0;
}
