/*
 * Copyright 2016 Nihlaeth
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
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

int parse_config(cron_rule_t **rules, char * filename) {
    int rule_index = 0;
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
        if (line[0] == '#') {
            continue;
        }
        char *savePtr = line;
        char *token = strtok_r(savePtr, " ", &savePtr);
        int index = 0;
        cron_rule_t * tmp_rule = malloc(sizeof(cron_rule_t)+1);
        init_rule(tmp_rule);
        while(token) {
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
                    if (strlen(tmp_rule->rule) + strlen(token) + 2 > MAX_RULE_LENGTH) {
                        syslog(
                            LOG_ERR,
                            "max rule length of %d exceeded",
                            MAX_RULE_LENGTH);
                        exit(EXIT_FAILURE);
                    }
                    // add a space
                    strncat(tmp_rule->rule, " ", MAX_RULE_LENGTH - 1);
                    // add token
                    strncat(tmp_rule->rule, token, MAX_RULE_LENGTH - 1);
                    // check if last char is a newline, if so, remove it
                    size_t len = strlen(tmp_rule->rule);
                    if (tmp_rule->rule[len -1] == '\n') {
                        tmp_rule->rule[len - 1] = '\0';
                    }
                    break;
            }
            token = strtok_r(savePtr, " ", &savePtr);
            index += 1;
        }
        if (index > 0 && index < 5) {
            syslog(LOG_ERR, "Not enough tokens in rule '%s'.", line);
            exit(EXIT_FAILURE);
        }
        if (index > 0) {
            /* make space for another pointer in rules */
            rules = realloc(rules, sizeof(cron_rule_t *) * (rule_index + 1));
            rules[rule_index] = tmp_rule;
            rule_index++;
        }
        else {
            free(tmp_rule);
        }
    }

    fclose(fp);
    if (line)
        free(line);
    return rule_index;
}

void init_rule(cron_rule_t *rule) {
    int i;
    for (i = 0; i < 60; i++) {
        rule->minutes[i] = false;
    }
    for (i = 0; i < 24; i++) {
        rule->hours[i] = false;
    }
    for (i = 0; i < 31; i++) {
        rule->days_of_month[i] = false;
    }
    for (i = 0; i < 12; i++) {
        rule->months[i] = false;
    }
    for (i = 0; i < 7; i++) {
        rule->days_of_week[i] = false;
    }
    strcpy(rule->rule, "");
}

bool rule_match(cron_rule_t *rule, time_t time) {
    struct tm * local_time = localtime(&time);
    if (
            rule->minutes[local_time->tm_min] &&
            rule->hours[local_time->tm_hour] &&
            rule->days_of_month[local_time->tm_mday - 1] &&
            rule->months[local_time->tm_mon] &&
            rule->days_of_week[local_time->tm_wday]) {
        return true;
    }
    return false;
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
    compile_regex(
        &r_subtoken,
        "^(([0-9]+|\\*)(-[0-9]+)?(/[0-9]+)?|[a-zA-Z][a-zA-Z][a-zA-Z])$");
    compile_regex(&r_subtoken_num, "^([0-9]+|\\*)$");
    compile_regex(&r_subtoken_range, "^[0-9]+-[0-9]+$");
    compile_regex(&r_subtoken_div, "^(\\*|[0-9]+)/[0-9]+$");
    compile_regex(&r_subtoken_range_div, "^[0-9]+-[0-9]+/[0-9]+$");
    compile_regex(&r_subtoken_str, "^[a-zA-Z][a-zA-Z][a-zA-Z]$");
}

void parse_subtoken(bool* return_values, int len, char *subtoken) {
    int i;
    /*
     * arrays are already zero initialized,
     * don't erase previous subtoken parses!
    for (i = 0; i < len; i++) {
        return_values[i] = false;
    }
    */
    /* check if subtoken is valid */
    if (!match_regex(&r_subtoken, subtoken)) {
        syslog(LOG_ERR, "'%s' is not a valid subtoken", subtoken);
        exit(EXIT_FAILURE);
    }

    if (match_regex(&r_subtoken_num, subtoken)) {
        /* eg 14, or * */
        if (subtoken[0] == '*') {
            for (i = 0; i < len; i++) {
                return_values[i] = true;
            }
        }
        else {
            /* assume subtoken is a number */
            char *ptr;
            long int number = strtol(subtoken, &ptr, 10);
            return_values[number] = true;
        }

    } else if (match_regex(&r_subtoken_range, subtoken)) {
        /* eg 1 - 5 */
        char * ptr_dash;
        char * ptr_end;
        long int number_1 = strtol(subtoken, &ptr_dash, 10);
        long int number_2 = strtol(&ptr_dash[1], &ptr_end, 10);;
        for (i = 0; i < len; i++) {
            if ( i >= number_1 && i <= number_2) {
                return_values[i] = true;
            }
        }
    } else if (match_regex(&r_subtoken_div, subtoken)) {
        /* eg 14/2 */
        char * ptr_dash;
        char * ptr_end;
        long int divisor;
        if (subtoken[0] == '*') {
            divisor = strtol(&subtoken[2], &ptr_end, 10);
            if (divisor == 0) {
                syslog(LOG_ERR, "division by zero is not allowed");
                exit(EXIT_FAILURE);
            }
            for (i = 1; i < len; i++) {
                if (i % divisor == 0) {
                    return_values[i] = true;
                }
            }
        } else { 
            long int number_1 = strtol(subtoken, &ptr_dash, 10);
            divisor = strtol(&ptr_dash[1], &ptr_end, 10);
            if (divisor == 0) {
                syslog(LOG_ERR, "division by zero is not allowed");
                exit(EXIT_FAILURE);
            }
            for (i = 1; i < len; i++) {
                if ( number_1 % divisor == 0 && i == number_1 / divisor) {
                    return_values[i] = true;
                }
            }
        }
    } else if (match_regex(&r_subtoken_range_div, subtoken)) {
        /* eg 1-20/3 */
        char * ptr_dash;
        char * ptr_slash;
        char * ptr_end;
        long int number_1 = strtol(subtoken, &ptr_dash, 10);
        long int number_2 = strtol(&ptr_dash[1], &ptr_slash, 10);
        long int divisor = strtol(&ptr_slash[1], &ptr_end, 10);
         if (divisor == 0) {
            syslog(LOG_ERR, "division by zero is not allowed");
            exit(EXIT_FAILURE);
        } 
        for (i = 1; i < len; i++) {
            if (i >= number_1 && i <= number_2 && i % divisor == 0) {
                return_values[i] = true;
            }
        }
    } else if (match_regex(&r_subtoken_str, subtoken)) {
        /* eg Mon, Tue, Jan */
        char *months[12] = {
            "Jan",
            "Feb",
            "Mar",
            "Apr",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Okt",
            "Nov",
            "Dec" };
        char *weekdays[7] = {
            "Sun",
            "Mon",
            "Tue",
            "Wed",
            "Thu",
            "Fri",
            "Sat"};
        int result = -1;
        int index;
        // check if subtoken is in months
        for (index = 0; index < 12; index ++) {
            if (strcasecmp(months[index], subtoken) == 0) {
                result = index;
                break;
            }
        }
        // check if subtoken is in weekdays
        for (index = 0; index < 7; index ++) {
            if (strcasecmp(weekdays[index], subtoken) == 0) {
                result = index;
                break;
            }
        }

        if (result == -1) {
            syslog(LOG_ERR, "'%s' not in known subtokens", subtoken);
            exit(EXIT_FAILURE);
        }
        return_values[result] = true;
    } else {
        /* unknown subtoken type */
        syslog(LOG_ERR, "could not parse subtoken '%s'", subtoken);
        exit(EXIT_FAILURE);
    }
}

void split_into_subtokens(bool* arr, int len, char* token) {
    char *savePtr = token;
    char *subtoken = strtok_r(savePtr, ",", &savePtr);
    while (subtoken) {
        parse_subtoken(arr, len, subtoken);
        subtoken = strtok_r(savePtr, ",", &savePtr);
    }
}

void parse_minutes(char *token, cron_rule_t *rule) {
    split_into_subtokens(rule->minutes, 60, token);
}

void parse_hours(char *token, cron_rule_t *rule) {
    split_into_subtokens(rule->hours, 24, token);
}

void parse_days_of_month(char *token, cron_rule_t *rule) {
    split_into_subtokens(rule->days_of_month, 31, token);
}

void parse_months(char *token, cron_rule_t *rule) {
    split_into_subtokens(rule->months, 12, token);
}

void parse_days_of_week(char *token, cron_rule_t *rule) {
    split_into_subtokens(rule->days_of_week, 7, token);
}

