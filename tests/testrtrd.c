/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "tap.h"
#include "../src/sockets.h"

void send_msg(int socketfd, char* msg) {
    if (send(socketfd, msg, strlen(msg) + 1, 0) == -1) {
        printf("Failed to send message |%s|\n", msg);
    } else {
        // printf("Sent message |%s|\n", msg);
    }
}

char* receive_msg(int socketfd) {
    int status;
    char buf[128];
    if ((status = recv(socketfd, &buf, 128, 0)) > 0) {
        buf[status] = '\0';
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
    char *result;
    send_msg(socketfd, "CURRENT\n");
    result = receive_msg(socketfd);
    is(result, "CURRENT ", "request current song (empty)");
    free(result);
    
    send_msg(socketfd, "REQFILE\n");
    result = receive_msg(socketfd);
    like(result, "FILE [a-z/A-Z0-9_\\-\\.]+", "request file");
    free(result);

    send_msg(socketfd, "CURRENT\n");
    result = receive_msg(socketfd);
    like(result, "CURRENT [a-z/A-Z0-9_\\-\\.]+", "request current song");
    free(result);

    send_msg(socketfd, "NEXT\n");
    result = receive_msg(socketfd);
    is(result, "ACK", "next");
    free(result);

    send_msg(socketfd, "YES?\n");
    result = receive_msg(socketfd);
    is(result, "NEXT", "fetch command after knock");
    free(result);

    send_msg(socketfd, "PAUSE\n");
    result = receive_msg(socketfd);
    is(result, "ACK", "pause");
    free(result);

    send_msg(socketfd, "STOP\n");
    result = receive_msg(socketfd);
    is(result, "ACK", "stop");
    free(result);

    send_msg(socketfd, "YES?\n");
    result = receive_msg(socketfd);
    is(result, "STOP", "fetch stop command");
    free(result);
}

void main() {
    FILE *in;
    /* start daemon */
    if (!(in = popen("../src/rtrd", "r"))) {
        /* could not execute command */
        exit(EXIT_FAILURE);
    }
    pclose(in);
    /* 
     * now give the daemon a sec to open up the socket server
     * FIXME: race condition
     */
    sleep(1);
    /* run actual tests */
    plan(8);
    client_socket("/tmp/rtrd", *client_conn);
    exit(exit_status());
}
