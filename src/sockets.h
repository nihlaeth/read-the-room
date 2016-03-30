/*
 * Copyright 2016 Nihlaeth
 */
#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

void report_error();
void client_socket(char* address, void (*functionPtr)(int));
void server_socket(char* address, void (*functionPtr)(int));

#endif // SOCKETS_H_
