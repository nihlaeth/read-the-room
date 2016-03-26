/*
 * Copyright 2016 Nihlaeth
 */

#ifndef DAEMONIZE_H_
#define DAEMONIZE_H_

#include <stdio.h> // stdin, stdout, stderr
#include <stdlib.h> // exit
#include <unistd.h> // fork, chdir, sysconf
#include <sys/stat.h> // umask
#include <syslog.h> // syslog, openlog, closelog
#include <fcntl.h> // O_RDWR, O_CREAT

#ifndef DAEMON_NAME
#define DAEMON_NAME "rtrd"
#endif

void signal_handler(int sig);
void daemonize();


#endif // DAEMONIZE_H_
