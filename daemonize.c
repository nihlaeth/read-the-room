#include <stdio.h>    //stdin, stdout, stderr
#include <stdlib.h>   //exit(3)
#include <unistd.h>   //fork(3), chdir(3), sysconf(3)
#include <signal.h>   //signal(3)
#include <sys/stat.h> //umask(3)
#include <syslog.h>   //syslog(3), openlog(3), closelog(3)
#include <fcntl.h>    //O_RDWR, O_CREAT
#include "daemonize.h"

/* signal handler function */
void signal_handler(int sig) 
{
    switch(sig){
        case SIGHUP:
            /* rehash the server */
            break;          
        case SIGTERM:
            /* finalize the server */
            exit(EXIT_SUCCESS);
            break;          
    }
}

void daemonize(char* name) {
    /* first fork */
    printf("first fork\n");
    pid_t pid = fork();
    if (pid < 0) { exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }

    /* set sid */
    printf("set sid\n");
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    /* Catch, ignore and handle signals */
    printf("register signal handlers\n");
    //TODO: Implement a working signal handler */
    signal(SIGHUP, signal_handler); /* hangup signal */
    signal(SIGTERM, signal_handler); /* software termination signal from kill */
    signal(SIGCHLD, SIG_IGN);
    //signal(SIGHUP, SIG_IGN);    
    
    /* second fork */
    printf("second fork\n");
    pid = fork();
    if (pid < 0) { exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }

    /* set file permissions */
    printf("set file permission mask\n");
    umask(0);

    /* chdir to safe directory */
    printf("chdir to root\n");
    chdir("/");

    /* close all open file descriptors */
    printf("closing all open file descriptors - bye!\n");
    int fd;
    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--)
    {
        close(fd);
    }

    /* open stdin, stdout and stderr */
    stdin = fopen("/dev/null", "r");
    stdout = fopen("/dev/null", "w+");
    stderr = fopen("/dev/null", "w+");

    /* lock pid file */
    int lfp = open("exampled.lock", O_RDWR|O_CREAT, 0640);
    if (lfp<0) {
        /* can not open */
        exit(1);
    }
    if (lockf(lfp, F_TLOCK, 0) < 0) {
        /* can not lock */
        exit(0);
        /* only first instance continues */
    }
    char str[10];
    sprintf(str, "%d\n", getpid());
    /* record pid to lockfile */
    write(lfp, str, 10);

    /* Open the log file */
    openlog(name, LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon started");
}
