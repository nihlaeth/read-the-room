/*
 * Copyright 2016 Nihlaeth
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include <iconv.h>
#include "./cronparser.h"


void pick_file(int rulesc, cron_rule_t **rules, char* file_name) {
    /*
     * read rules, select file, store file name in filename
     */
    time_t current;
    time(&current);
    syslog(LOG_INFO, "check 5");
    /* we will have at most rulesc matches */
    cron_rule_t **matching_rules = malloc(sizeof(cron_rule_t *) * rulesc);
    int match_index = 0;

    syslog(LOG_INFO, "check 5.0");
    int i;
    for (i = 0; i < rulesc; i++) {
        syslog(LOG_INFO, "see if rule matches, i=%d", i);
        syslog(LOG_INFO, "pointers: rules[i]=%d, current=%d", &rules[i], &current);
        if (rule_match(rules[i], current)) {
            syslog(LOG_INFO, "rule matches");
            matching_rules[match_index] = rules[i];
            match_index++;
        }
        else {
            syslog(LOG_INFO, "no match");
        }
    }

    syslog(LOG_INFO, "check 5.1");
    /* get total length of rules */
    size_t len = 0;
    for (i = 0; i < match_index; i++) {
        len += strlen(matching_rules[i]->rule);
    }

    syslog(LOG_INFO, "check 5.2");
    /* concat all matching rules */
    char *all_rules = malloc(len + 1);
    strcpy(all_rules, "");

    syslog(LOG_INFO, "check 5.3");
    for (i = 0; i < match_index; i++) {
        strncat(all_rules, matching_rules[i]->rule, len);
    }

    syslog(LOG_INFO, "check 6");
    /* 
     * arrays for tags -> we use them to construct the tmsu command
     * 
     * in the most extreme, a tag has length 3 (a space, an indication
     * of how it's to be interpreted, and a single char for the tag
     * itself. So we need to allocate space for at most
     * strlen(all_rules) / 3
     */
    size_t size_of_pointers = strlen(all_rules) / 3 + 1;
    char **mandatory_tags = malloc(size_of_pointers);
    int mandatoryc = 0;
    char **optional_tags = malloc(size_of_pointers);
    int optionalc = 0;
    char **exclusion_tags = malloc(size_of_pointers);
    int exclusionc = 0;

    /* loop through tags */
    char *savePtr = all_rules;
    char *tag = strtok_r(savePtr, " ", &savePtr);
    while (tag) {
        /* check starting char */
        if (strlen(tag) == 0) {
            continue;
        }
        switch (tag[0]) {
            case '+':
                /* mandatory tag */
                mandatory_tags[mandatoryc] = malloc(strlen(tag));
                strcpy(mandatory_tags[mandatoryc], &tag[1]);
                mandatoryc++;
                break;
            case '-':
                /* exclusion tag */
                exclusion_tags[exclusionc] = malloc(strlen(tag));
                strcpy(exclusion_tags[exclusionc], &tag[1]);
                exclusionc++;
                break;
            case '?':
                /* optional tag */
                optional_tags[optionalc] = malloc(strlen(tag));
                strcpy(optional_tags[optionalc], &tag[1]);
                optionalc++;
                break;
            default:
                /* invalid tag */
                syslog(
                    LOG_ERR,
                    "Tags have to start with +, - or ?, got |%s| instead.",
                    tag);
                exit(EXIT_FAILURE);
                break;
        }
        tag = strtok_r(savePtr, " ", &savePtr);
    }

    /* construct command */
    char *cmd = malloc(strlen("tmsu files ") + 1);
    strcpy(cmd, "tmsu files ");
    /* 0 == none, 1 == mandatory, 2 == optional, 3 == exclusion */
    int last_tag = 0;
    size_t new_cmd_len;

    syslog(LOG_INFO, "check 7");
    /* handle all optional tags */
    for (i = 0; i < optionalc; i++) {
        switch (last_tag) {
            case 0:
                /* first tag of this command, no prefix necessary */
                new_cmd_len = strlen(cmd) + strlen(optional_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, optional_tags[i], new_cmd_len);
                break;
            case 1:
                /* mandatory tag, should not happen but who knows
                 * what we might change in the future. prepend " and "
                 */
            case 3:
                /* exclusion tag, should not happen but who knows
                 * what we might change in the futute. prepend " and "
                 */
                new_cmd_len = strlen(cmd) + strlen(" and ") + strlen(optional_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, " and ", new_cmd_len);
                strncat(cmd, optional_tags[i], new_cmd_len);
                break;
            case 2:
                /* optional tag, prepend " or " */
                new_cmd_len = strlen(cmd) + strlen(" or ") + strlen(optional_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, " or ", new_cmd_len);
                strncat(cmd, optional_tags[i], new_cmd_len);
                break;
            default:
                /* should not happen */
                syslog(
                    LOG_ERR,
                    "Unknown preceding tag type %d, cmd = |%s|",
                    last_tag,
                    cmd);
                exit(EXIT_FAILURE);
                break;
        }
        last_tag = 2;
    }

    /* handle all mandatory tags */
    for (i = 0; i < mandatoryc; i++) {
        switch (last_tag) {
            case 0:
                /* first tag of this command, no prefix necessary */
                new_cmd_len = strlen(cmd) + strlen(mandatory_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, mandatory_tags[i], new_cmd_len);
                break;
            case 1:
                /* mandatory tag, prefix " and " */
            case 2:
                /* optional tag, prefix " and " */
            case 3:
                /* exclusion tag, prefix " and " */
                new_cmd_len = strlen(cmd) + strlen(" and ") + strlen(mandatory_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, " and ", new_cmd_len);
                strncat(cmd, mandatory_tags[i], new_cmd_len);
                break;
            default:
                /* should not happen */
                syslog(
                    LOG_ERR,
                    "Unknown preceding tag type %d, cmd = |%s|",
                    last_tag,
                    cmd);
                exit(EXIT_FAILURE);
                break;
        }
        last_tag = 1;
    }

    /* handle exclusion tags */
    for (i = 0; i < exclusionc; i++) {
        switch (last_tag) {
            case 0:
                /* first tag of this command, prefix "not " */
                new_cmd_len = strlen(cmd) + strlen("not ") + strlen(exclusion_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, "not ", new_cmd_len);
                strncat(cmd, exclusion_tags[i], new_cmd_len);
                break;
            case 1:
                /* mandatory tag, prefix " and not " */
            case 2:
                /* optional tag, prefix " and not " */
            case 3:
                /* exclusions tag, prefix " and not " */
                new_cmd_len = strlen(cmd) + strlen(" and not ") + strlen(exclusion_tags[i]);
                cmd = realloc(cmd, new_cmd_len + 1);
                strncat(cmd, " and not ", new_cmd_len);
                strncat(cmd, exclusion_tags[i], new_cmd_len);
                break;
            default:
                /* should not happen */
                syslog(
                    LOG_ERR,
                    "Unknown preceding tag type %d, cmd = |%s|",
                    last_tag,
                    cmd);
                exit(EXIT_FAILURE);
                break;
        }
        last_tag = 3;
    }
    
    /* add the shuf command */
    char *shuf = " 2>&1 | shuf -n 1 2>&1";
    cmd = realloc(cmd, strlen(cmd) + strlen(shuf) + 1);
    strncat(cmd, shuf, strlen(cmd) + strlen(shuf));

    /* now let's fetch a file list, and have shuf pick a random line */
    FILE *in;
    char buff[512];
    char *output = malloc(sizeof(char));
    output[0] = '\0';

    if (!(in = popen(cmd, "r"))) {
        syslog(LOG_ERR, "Could not execute command |%s|", cmd);
        exit(EXIT_FAILURE);
    }
    while(fgets(buff, sizeof(buff), in) != NULL) {
        output = realloc(output, strlen(output) + strlen(buff) + 1);
        strncat(output, buff, strlen(output) + strlen(buff));
    }
    if (ferror(in) != 0 || pclose(in) != 0) {
        /* only the exit code of the last command is shown,
         * so tmsu errors are ignored */
        syslog(LOG_ERR, "Command execution failed |%s|", cmd);
        exit(EXIT_FAILURE);
    } else {
        /* we got a file name! or a tmsu error... */
        if (output[0] != '.' && output[0] != '/') {
            syslog(LOG_ERR, "Looks like |%s| failed to execute: |%s|", cmd, output);
            exit(EXIT_FAILURE);
        }
        /* remove trailing newline */
        int index = strlen(output) - 1;
        if (output[index] == '\n') {
            output[index] = '\0';
        }
        if (output[index - 1] == '\r') {
            output[index - 1] = '\0';
        }
    }


    syslog(LOG_INFO, "check 8");
    /* free up all the dynamically allocated memory */
    free(cmd);
    free(matching_rules);
    free(all_rules);
    for (i = 0; i < mandatoryc; i++) {
        free(mandatory_tags[i]);
    }
    for (i = 0; i < exclusionc; i++) {
        free(exclusion_tags[i]);
    }
    for (i = 0; i < optionalc; i++) {
        free(optional_tags[i]);
    }
    free(mandatory_tags);
    free(optional_tags);
    free(exclusion_tags);

    syslog(LOG_INFO, "check 9");
    file_name = realloc(file_name, strlen(output) + 1);
    strcpy(file_name, output);
    free(output);
}
