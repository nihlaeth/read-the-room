/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "./pickfile.h"
#include "./cronparser.h"
#include "./sockets.h"
#include "./daemonize.h"

cron_rule_t **rules;
int rulesc;
char *current_song;
/* we could use a queue here, but that's a lot of hasle
 * for something we will probably never actually use
 * for now, if a new message needs to be queued before
 * the old one is fetched, we simply overwrite it.
 */
char *queued_message;

void send_ack(int socketfd) {
    if (send(socketfd, "ACK\n", strlen("ACK\n") + 1, 0) == -1) {
        syslog(LOG_WARNING, "Failed to send ACK");
    }
}

void handle_message(int socketfd, char *msg) {
    /* identify message and formulate a response */

    /* 
     * possible messages
     *
     * IDENT (read-the-room|rtr-jack)
     * PAUSE
     * PLAY
     * NEXT
     * CURRENT
     * STOP
     * REQFILE
     * YES?
     */
    if (strcasecmp(msg, "PAUSE") == 0) {
        queued_message = realloc(queued_message, strlen("PAUSE\n") + 1);
        strcpy(queued_message, "PAUSE\n");
        // send SIGUSR1
        send_ack(socketfd);
    } else if (strcasecmp(msg, "PLAY") == 0) {
        queued_message = realloc(queued_message, strlen("PLAY\n") + 1);
        strcpy(queued_message, "PLAY\n");
        // send SIGUSR1
        send_ack(socketfd);
    } else if (strcasecmp(msg, "NEXT") == 0) {
        queued_message = realloc(queued_message, strlen("NEXT\n") + 1);
        strcpy(queued_message, "NEXT\n");
        // send SIGUSR1
        send_ack(socketfd);
    } else if (strcasecmp(msg, "CURRENT") == 0) {
        size_t len = strlen(current_song) + strlen("CURRENT \n");
        char* tmp_msg = malloc(len + 1);
        snprintf(tmp_msg, len, "CURRENT %s\n", current_song);
        if (send(socketfd, tmp_msg, len + 1, 0) == -1) {
            syslog(LOG_WARNING, "Failed to send current playing song.");
        }
        free(tmp_msg);
    } else if (strcasecmp(msg, "STOP") == 0) {
        queued_message = realloc(queued_message, strlen("STOP\n") + 1);
        strcpy(queued_message, "STOP\n");
        // send SIGUSR1
        // at ack, also shut down this daemon!
        send_ack(socketfd);
    } else if (strcasecmp(msg, "REQFILE") == 0) {
        pick_file(rulesc, rules, current_song);
        syslog(LOG_INFO, "Current song: |%s|", current_song);
        size_t len = strlen(current_song) + strlen("FILE \n");
        char* tmp_msg = malloc(len + 1);
        snprintf(tmp_msg, len, "FILE %s\n", current_song);
        if (send(socketfd, tmp_msg, len + 1, 0) == -1) {
            syslog(LOG_WARNING, "Failed to send new song.");
        }
        free(tmp_msg);
    } else if (strcasecmp(msg, "YES?") == 0) {
        /* send message from queue */
        if (send(socketfd, queued_message, strlen(queued_message) + 1, 0) == -1) {
            syslog(LOG_WARNING, "Failed to send queued message |%s|", queued_message);
        }
        if (strcasecmp(queued_message, "STOP\n") == 0) {
            // shut down this daemon too!
            syslog(LOG_INFO, "Shutting down rtrd");
            exit(EXIT_SUCCESS);
        }
        else {
            strcpy(queued_message, "");
            queued_message = realloc(queued_message, sizeof(char));
        }
    } else {
        /* unknown message */
        syslog(LOG_WARNING, "Unknown message |%s|", msg);
    }
}

void server_connection(int socketfd) {
    /* 0 means unknown, 1 = read-the-room, 2 = rtr-jack */
    char buf[128];
    int len;
    char *msg = malloc(sizeof(char));
    strcpy(msg, "");
    while (len = recv(socketfd, &buf, 128, 0), len > 0) {
        if (len == -1) {
            /* client dropped off */
            break;
        }
        /* 
         * handle message 
         * first, check if there's a newline in the buffer,
         * if not, just append buf to msg
         * if so, that ends the message (handle it!)
         * and add rest to msg if there is any
         * (we don't check for newlines there,
         * this is not a high-speed data transfer)
         */
        char *newline = strchr(buf, '\n');
        if (newline == NULL) {
            /* no newline in buffer, incomplete command */
            msg = realloc(msg, strlen(msg) + strlen(buf) + 1);
            strncat(msg, buf, strlen(msg) + strlen(buf));
        } else {
            char *rest = &newline[1];
            newline[0] = '\0';
            msg = realloc(msg, strlen(msg) + strlen(buf) + 1);
            strncat(msg, buf, strlen(msg) + strlen(buf));
            handle_message(socketfd, msg);
            
            /* figure out if the remaining message is worth the trouble */
            if (strlen(rest) > 1) {
                msg = realloc(msg, strlen(rest) + 1);
                strcpy(msg, rest);
            } else {
                msg = realloc(msg, sizeof(char));
                strcpy(msg, "");
            }
        }
    }
}

void main() {
    /* daemonize process */
    daemonize("rtrd");

    /* init some data */
    queued_message = malloc(sizeof(char *));
    strcpy(queued_message, "");
    current_song = malloc(sizeof(char));
    strcpy(current_song, "");

    /* load rules from config */
    regex_init();
    rules = malloc(sizeof(cron_rule_t *));
    rulesc = parse_config(rules, "/git/read-the-room/testconfig");

    /* listen to socket */
    server_socket("/tmp/rtrd", *server_connection);
    
    /* free memory */
    free(queued_message);
    free(current_song);
    int i;
    for (i = 0; i < rulesc; i++) {
        free(rules[i]);
    }
    free(rules);
    exit(EXIT_SUCCESS);
}
