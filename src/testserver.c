/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "sockets.h"

void server_conn(int socketfd) {
    printf("Client connected\n");
    char buf[100];
    int len;
    while (len = recv(socketfd, &buf, 100, 0), len > 0) {
        if (len == -1) {
            printf("Client dropped off\n");
            break;
        }
        printf("Received message |%s|\n", buf);
    }
    printf("Closing socket\n");
}

void main() {
    printf("Starting server\n");
    server_socket("/tmp/testsocket", *server_conn);
    printf("Stopped server\n");
}
