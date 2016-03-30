/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "sockets.h"

void send_msg(int socketfd, char* msg) {
    if (send(socketfd, msg, strlen(msg) + 1, 0) == -1) {
        printf("Failed to send message |%s|\n", msg);
    } else {
        printf("Sent message |%s|\n", msg);
    }
}

char* receive_msg(int socketfd) {
    int status;
    char buf[128];
    if ((status = recv(socketfd, &buf, 128, 0)) > 0) {
        buf[status] = '\0';
        printf("Received message |%s|\n", buf);
        char *return_str = malloc(strlen(buf) + 1);
        strcpy(return_str, buf);
        return return_str;
    } else {
        if (status < 0) {
            printf("Could not receive message.\n");
        } else {
            printf("Server closed connection.\n");
            exit(EXIT_SUCCESS);
        }
    }
}

void client_conn(int socketfd) {
    send_msg(socketfd, "CURRENT\n");
    receive_msg(socketfd);
    send_msg(socketfd, "REQFILE\n");
    receive_msg(socketfd);
    send_msg(socketfd, "CURRENT\n");
    receive_msg(socketfd);
    send_msg(socketfd, "NEXT\n");
    receive_msg(socketfd);
    send_msg(socketfd, "YES?\n");
    receive_msg(socketfd);
    send_msg(socketfd, "PAUSE\n");
    receive_msg(socketfd);
    send_msg(socketfd, "STOP\n");
    receive_msg(socketfd);
    send_msg(socketfd, "YES?\n");
    receive_msg(socketfd);
}

void main() {
    printf("Starting client\n");
    client_socket("/tmp/rtrd", *client_conn);
    printf("Stopped client\n");
    exit(EXIT_SUCCESS);
}
