/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "sockets.h"

void client_conn(int socketfd) {
    char *msg = "testmessage";
    if (send(socketfd, msg, strlen(msg) + 1, 0) == -1) {
        printf("Failed to send message\n");
    } else {
        printf("Sent message\n");
    }
}

void main() {
    printf("Starting client\n");
    client_socket("/tmp/testsocket", *client_conn);
    printf("Stopped client\n");
}
