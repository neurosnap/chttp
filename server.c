#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

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

void httpHeaderDate(char *date, struct tm *time) {
    //Date: Sun, 17 Jan 2016 19:19:37 GMT\n
    char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *month[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    sprintf(
        date,
        "Date: %s, %2d %s %d %2d:%2d:%2d GMT\n",
        day[time->tm_wday],
        time->tm_mday,
        month[time->tm_mon],
        time->tm_year + 1900,
        time->tm_hour,
        time->tm_hour,
        time->tm_sec
    );
}

char *getStatus(int code) {
    switch(code) {
        case 400:
            return "Bad Request";
            break;

        case 500:
            return "Internal Server Error";
            break;

        case 502:
            return "Bad Gateway";
            break;

        case 503:
            return "Service Unavailable";
            break;

        case 200:
        default:
            return "OK";
            break;
    }
}

void sendStatus(int sock, int code) {
    char *reason = getStatus(code);

    int reasonLen = strlen(reason);
    char http_proto[14 + reasonLen];
    char http_status[13 + reasonLen];

    printf("%d", reasonLen);

    sprintf(http_proto, "HTTP/1.1 %3d %s\n", code, reason);
    sprintf(http_status, "Status: %3d %s\n", code, reason);

    puts(http_proto);
    puts(http_status);

    send(sock, http_proto, sizeof(http_proto), 0);
    send(sock, http_status, sizeof(http_status), 0);
}

void sendHeaders(int sock) {
    time_t t = time(NULL);
    struct tm tm = *gmtime(&t);
    char date[42];
    httpHeaderDate(date, &tm);

    send(sock, "Server: chttp\n", 14, 0);
    send(sock, "Connection: close\n", 18, 0);
    send(sock, date, strlen(date), 0);
}

int main(int argc, char *argv[]) {
    char *port = argv[1];
    if (port == NULL) {
        printf("First argument needs to be the port, e.g.: 1337\n");
        return 1;
    }

    struct sockaddr_in client;
    unsigned int client_len;
    int client_socket;
    int max_connections = 1000;

    int http_socket = httpListen(atoi(port), max_connections);

    int bufSize = 1024 * 4;
    char *buffer = malloc(bufSize);
    for (;;) {
        client_len = sizeof(client);
        client_socket = accept(http_socket, (struct sockaddr *) &client, &client_len);
        if (client_socket == -1) {
            continue;
        }

        printf("CLIENT CONNECTED: %d\n", client_socket);

        recv(client_socket, buffer, bufSize, 0);

        //printf("Message from client:\n%s\n", buffer);

        sendHeaders(client_socket);
        sendStatus(client_socket, 200);

        send(client_socket, "Allow: GET\n", 11, 0);
        send(client_socket, "Content-Type: text/html\n", 24, 0);
        send(client_socket, "Content-Length: 3\n", 18, 0);

        send(client_socket, "\n", 1, 0);

        send(client_socket, "Hi!", 3, 0);
        close(client_socket);

        printf("CLIENT SOCKET CLOSED\n");
    }

    free(buffer);
    return 0;
}
