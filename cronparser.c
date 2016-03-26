/*
 * Copyright 2016 Nihlaeth
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include "cronparser.h"

/*
typedef struct {
    bool minutes[60];
    bool hours[24];
    bool days_of_month[31];
    bool month[12];
    bool days_of_week[7];
    char *rule;
} cront_rule_t;
*/

cron_rule_t * parse_config(char * filename) {
    cron_rule_t * rules;

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "Could not open config file '%s'.", filename);
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
        if (line[0] == '#') {
            continue;
        }
        char *token = strtok(line, " ");
        int index = 0;
        while(token) {
            printf("Token %s\n", token);
            /*
            switch (index) {
                case 0:
                    parse_minutes(token, tmp_rule);
                    break;
                case 1:
                    parse_hours(token, tmp_rule);
                    break;
                case 2:
                    parse_days_of_month(token, tmp_rule);
                    break;
                case 3:
                    parse_month(token, tmp_rule);
                    break;
                case 4:
                    parse_days_of_week(token, tmp_rule);
                    break;
                default:
                    // TODO: add token to rule
                    break;
            }
            */
            token = strtok(NULL, " ");
            index += 1;
        }
        if (index > 0 && index < 5) {
            syslog(LOG_ERR, "Not enough tokens in rule '%s'.", line);
            exit(EXIT_FAILURE);
        }
        if (index > 0) {
            // TODO: add rule to rules
        }
    }

    fclose(fp);
    if (line)
        free(line);
    return rules;
}

bool rule_match(cron_rule_t rule, time_t time) {

}


