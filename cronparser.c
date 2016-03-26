/*
 * Copyright 2016 Nihlaeth
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <regex.h>
#include "cronparser.h"
#define MAX_ERROR_MSG 0x1000
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
                    parse_months(token, tmp_rule);
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

void compile_regex(regex_t *r, const char *regex_text) {
    int status = regcomp(r, regex_text, REG_EXTENDED|REG_NEWLINE);
    if (status != 0) {
        char error_message[MAX_ERROR_MSG];
        regerror(status, r, error_message, MAX_ERROR_MSG);
        syslog(
            LOG_ERR,
            "Regex error compiling '%s': %s\n",
            regex_text,
            error_message);
        exit(EXIT_FAILURE);
    }
}

bool match_regex(regex_t *r, const char *to_match) {
    /* "P" is a pointer into the string which points to the end of the
       previous match. */
    const char * p = to_match;
    /* "N_matches" is the maximum number of matches allowed. */
    const int n_matches = 10;
    /* "M" contains the matches found. */
    regmatch_t m[n_matches];

    int nomatch = regexec(r, p, n_matches, m, 0);
    if (nomatch) {
        return false;
    }
    return true;
}

regex_t r_subtoken;
regex_t r_subtoken_num;
regex_t r_subtoken_range;
regex_t r_subtoken_div;
regex_t r_subtoken_range_div;
regex_t r_subtoken_str;

void regex_init() {
    compile_regex(&r_subtoken, "^([0-9]+|\\*)(-[0-9]+)?(/[0-9]+)?$");
    compile_regex(&r_subtoken_num, "^([0-9]+|\\*)$");
    compile_regex(&r_subtoken_range, "^[0-9]+-[0-9]+$");
    compile_regex(&r_subtoken_div, "^(\\*|[0-9]+)/[0-9]+$");
    compile_regex(&r_subtoken_range_div, "^[0-9]+-[0-9]+/[0-9]+$");
    compile_regex(&r_subtoken_str, "^[a-zA-Z][a-zA-Z][a-zA-Z]$");
}

int* parse_subtoken(char *subtoken) {
    /* a subtoken should translate to at most 31 distinct values,
     * since none of the tokens have more than 31 distinct values.
     */
    int values[31];

}

