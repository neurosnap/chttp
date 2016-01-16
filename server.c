#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

int httpListen(int port, int max_connections) {
    struct sockaddr_in server;
    int http_socket;

    http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1) {
        fprintf(stderr, "Socket Error\n");
        return 1;
    }

    printf("WE MADE A SOCKET CONNECTION: %d\n", http_socket);

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(http_socket, (struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "Could not bind to port, it might already be in use: %d\n", port);
        return 1;
    }

    printf("WE MADE A PORT CONNECTION: %d\n", port);

    if (listen(http_socket, max_connections) == -1) {
        fprintf(stderr, "Could not listen on socket\n");
        return 1;
    }

    printf("WE ARE LISTENING ON SOCKET\n");

    return http_socket;
}

int main(int argc, char *argv[]) {
    char *port = argv[1];
    struct sockaddr_in client;
    unsigned int client_len;
    int client_socket;
    int max_connections = 1000;
    int http_socket = httpListen(atoi(port), max_connections);
    time_t t = time(NULL);

    int bufSize = 1024 * 6;
    char *buffer = malloc(bufSize);
    for (;;) {
        char date[33] = "Date: ";
        sprintf(date, "%s\n", asctime(gmtime(&t)));

        client_len = sizeof(client);
        client_socket = accept(http_socket, (struct sockaddr *) &client, &client_len);
        if (client_socket == -1) {
            continue;
        }

        printf("CLIENT CONNECTED: %d\n", client_socket);

        recv(client_socket, buffer, bufSize, 0);

        //printf("Message from client:\n%s\n", buffer);

        send(client_socket, "HTTP/1.1 200 OK\n", 16, 0);
        send(client_socket, "Server: chttp\n", 14, 0);
        send(client_socket, "Content-Length: 3\n", 18, 0);
        send(client_socket, "Status: 200 OK\n", 15, 0);
        send(client_socket, "Connection: close\n", 18, 0);
        //send(client_socket, date, 33, 0);
        send(client_socket, "Content-Type: text/html\n\n", 25, 0);

        send(client_socket, "Hi!", 3, 0);
        close(client_socket);

        printf("CLIENT SOCKET CLOSED\n");
    }

    free(buffer);
    return 0;
}
