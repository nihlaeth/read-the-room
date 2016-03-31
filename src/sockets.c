/*
 * Copyright 2016 Nihlaeth
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include "./sockets.h"

void report_error() {
    switch (errno) {
        case EACCES:
            syslog(LOG_ERR, "EACCES Write permission denied.");
            break;
        case EPERM:
            syslog(LOG_ERR, "EPERM Invalid credentials.");
            break;
        case EADDRINUSE:
            syslog(LOG_ERR, "EADDRINUSE Local address already in use.");
            break;
        case EAFNOSUPPORT:
            syslog(LOG_ERR, "EAFNOSUPPORT The passed address didn't have the correct address family.");
            break;
        case EAGAIN:
            syslog(LOG_ERR, "EAGAIN No more free local ports.");
            break;
        case EALREADY:
            syslog(LOG_ERR, "EALREADY Previous connection in progress.");
            break;
        case EBADF:
            syslog(LOG_ERR, "EBADF Bad file descriptor.");
            break;
        case ECONNREFUSED:
            syslog(LOG_ERR, "ECONNREFUSED No one's listening.");
            break;
        case EFAULT:
            syslog(LOG_ERR, "EFAULT Address is outside of user's address space.");
            break;
        case EINPROGRESS:
            syslog(LOG_ERR, "EINPROGRESS Socket is nonblocking and connection cannot be completed immediately.");
            break;
        case EINTR:
            syslog(LOG_ERR, "EINTR System call was interrupted by signal.");
            break;
        case EISCONN:
            syslog(LOG_ERR, "EISCONN The socket is already connected.");
            break;
        case ENETUNREACH:
            syslog(LOG_ERR, "ENETUNREACH Network unreachable.");
            break;
        case ENOTSOCK:
            syslog(LOG_ERR, "ENOTSOCK Filedescriptor is not a socket.");
            break;
        case ETIMEDOUT:
            syslog(LOG_ERR, "ETIMEDOUT Timeout while attemting connection, server may be too busy.");
            break;
        case ECONNRESET:
            syslog(LOG_ERR, "ECONNRESET Remote connection socket was unexpectedly closes.");
            break;
        case EINVAL:
            syslog(LOG_ERR, "EINVAL Invalid argument passed.");
            break;
        case ENOENT:
            syslog(LOG_ERR, "ENOENT Specified pathname does not exist.");
            break;
        case ENOMEM:
            syslog(LOG_ERR, "ENOMEM Out of memory.");
            break;
        case ENOTCONN:
            syslog(LOG_ERR, "ENOTCONN Socket address needs a target address, but socket is not connected.");
            break;
        case EOPNOTSUPP:
            syslog(LOG_ERR, "EOPNOTSUPP Stream operation called on non-stream oriented socket.");
            break;
        case EPIPE:
            syslog(LOG_ERR, "EPIPE Remote socket was closed on a stream socket.");
            break;
        case EPROTONOSUPPORT:
            syslog(LOG_ERR, "EPROTONOSUPPORT Protocol not supported.");
            break;
        case EPROTOTYPE:
            syslog(LOG_ERR, "EPROTOTYPE Remote socket type does not match local socket type.");
            break;
        case ESOCKTNOSUPPORT:
            syslog(LOG_ERR, "ESOCKTNOSUPPORT Unknown socket type.");
            break;
        default:
            syslog(LOG_ERR, "Unknown problem");
            break;
    }
}

void client_socket(char* address, void (*functionPtr)(int)) {
    struct sockaddr_un remote;
   
    /* create socket */ 
    int socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketfd == -1) {
        syslog(LOG_ERR, "Could not create socket.");
        report_error();
        exit(EXIT_FAILURE);
    }

    /* connect to socket */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, address);
    int status = connect(socketfd, (struct sockaddr *)&remote, sizeof(remote));
    if (status == -1) {
        syslog(LOG_ERR, "Could not listen at socket.");
        report_error();
        exit(EXIT_FAILURE);
    }

    (*functionPtr)(socketfd);
    /* example of what this function might do:
    while(printf("> "), fgets(str, 100, stdin), !feof(stdin)) {
        if (send(s, str, strlen(str), 0) == -1) {
            perror("send");
            exit(1);
        }

        if ((t=recv(s, str, 100, 0)) > 0) {
            str[t] = '\0';
            printf("echo> %s", str);
        } else {
            if (t < 0) perror("recv");
            else printf("Server closed connection\n");
            exit(1);
        }
    }
    */
    close(socketfd);
}

void server_socket(char* address, void (*functionPtr)(int)) {
    /*
     * functionPtr is called for every client that connects
     */
    struct sockaddr_un local, remote;

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        syslog(LOG_ERR, "Could not create socket.");
        report_error();
        exit(EXIT_FAILURE);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, address);
    unlink(local.sun_path);
    int local_len = sizeof(local);
    int status = bind(sockfd, (struct sockaddr *)&local, local_len);
    if (status == -1) {
        syslog(LOG_ERR, "Could not bind socket to address.");
        report_error();
        exit(EXIT_FAILURE);
    }

    status = listen(sockfd, 5);
    if (status == -1) {
        syslog(LOG_ERR, "Could not listen at socket.");
        report_error();
        exit(EXIT_FAILURE);
    }

    while (true) {
        /* accept connection */
        int remote_len = sizeof(remote);
        int connectedfd = accept(sockfd, (struct sockaddr *)&remote, &remote_len);
        if (connectedfd == -1) {
            syslog(LOG_WARNING, "Failed to connect to client.");
            report_error();
        } else {
            (*functionPtr)(connectedfd);
        }
        /* example of what this function might do:
        char* buf[100];
        while (len = recv(connectedfd, &buf, 100, 0), len > 0) {
            if (len == -1) {
                // we expect client to drop off, don't log an err here
                break;
            }
            status = send(connectedfd, &buf, len, 0);
            if (status == -1) {
                syslog(LOG_WARNING, "Could not send message to client.");
            }
        }
        */
        status = close(connectedfd);
        if (status == -1) {
            syslog(LOG_WARNING, "Could not close connection with client.");
            report_error();
        }
    }
    close(sockfd);
    unlink(local.sun_path);
}
