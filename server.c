#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in server, client;

    int http_socket, client_socket, bound, hear;
    unsigned int client_len;
    int port = 1337;
    int max_connections = 5;

    http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1) {
        fprintf(stderr, "Socket Error\n");
        return 1;
    }

    printf("WE MADE A SOCKET CONNECTION: %d\n", http_socket);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    bound = bind(http_socket, (struct sockaddr *) &server, sizeof(server));
    if (bound == -1) {
        fprintf(stderr, "Could not bind to port, it might already be in use: %d\n", port);
        return 1;
    }

    printf("WE MADE A PORT CONNECTION: %d\n", port);

    hear = listen(http_socket, max_connections);
    if (hear == -1) {
        fprintf(stderr, "Could not listen on socket\n");
    }

    printf("WE ARE LISTENING ON SOCKET\n");

    char buffer[256];
    for (;;) {
        client_len = sizeof(client);
        client_socket = accept(http_socket, (struct sockaddr *) &client, &client_len);

        printf("CLIENT CONNECTED: %d\n", client_socket);

        recv(client_socket, buffer, 256, 0);

        printf("Message from client: %s\n", buffer);

        close(client_socket);

        printf("CLIENT SOCKET CLOSED\n");
    }

    return 0;
}
