/*
 * Copyright 2016 Nihlaeth
 */

#ifndef SRC_DAEMONIZE_H_
#define SRC_DAEMONIZE_H_

#include <stdio.h> // stdin, stdout, stderr
#include <stdlib.h> // exit
#include <unistd.h> // fork, chdir, sysconf
#include <signal.h>
#include <sys/stat.h> // umask
#include <syslog.h> // syslog, openlog, closelog
#include <fcntl.h> // O_RDWR, O_CREAT

void signal_handler(int sig);
void daemonize(char* name);

#endif // SRC_DAEMONIZE_H_
