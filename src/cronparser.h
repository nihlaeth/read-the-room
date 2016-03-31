/*
 * Copyright 2016 Nihlaeth
 */

#ifndef SRC_CRONPARSER_H_
#define SRC_CRONPARSER_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <regex.h>
#include <time.h>
#include "./cronparser.h"
#include "./memory.h"

typedef struct cron_rule_t {
    bool minutes[60];
    bool hours[24];
    bool days_of_month[31];
    bool months[12];
    bool days_of_week[7];
    char *rule;
} cron_rule_t;

typedef struct rule_container_t {
    cron_rule_t *arr;
    int num_rules;
} rule_container_t;

void parse_config(rule_container_t *rules, char *filename);

bool rule_match(cron_rule_t *rule, time_t time);

void init_rule(cron_rule_t *rule);
void copy_rule(cron_rule_t *dest, cron_rule_t *origin);

void compile_regex(regex_t *r, const char *regex_text);
bool match_regex(regex_t *r, const char *to_match);
void regex_init();
regex_t r_subtoken;
regex_t r_subtoken_num;
regex_t r_subtoken_range;
regex_t r_subtoken_div;
regex_t r_subtoken_range_div;
regex_t r_subtoken_str;

void split_into_subtokens(bool* arr, int len, char* token);
void parse_minutes(char *token, cron_rule_t *rule);
void parse_hours(char *token, cron_rule_t *rule);
void parse_days_of_month(char *token, cron_rule_t *rule);
void parse_months(char *token, cron_rule_t *rule);
void parse_days_of_week(char *token, cron_rule_t *rule);
void parse_subtoken(bool* return_values, int len, char *subtoken);

#endif // SRC_CRONPARSER_H_
