/*
 * Copyright 2016 Nihlaeth
 */

#ifndef SRC_CRONPARSER_H_
#define SRC_CRONPARSER_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>
#include "memory.h"

typedef struct cron_rule_t {
    bool *minutes;
    bool *hours;
    bool *days_of_month;
    bool *months;
    bool *days_of_week;
    char *rule;
} cron_rule_t;

int parse_config(cron_rule_t **rules, char *filename);

bool rule_match(cron_rule_t *rule, time_t time);

void init_rule(cron_rule_t *rule);
void free_rule(cron_rule_t *rule);

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
