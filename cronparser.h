/*
 * Copyright 2016 Nihlaeth
 */

#ifndef CRONPARSER_H_
#define CRONPARSER_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>

typedef struct cron_rule_t {
    bool minutes[60];
    bool hours[24];
    bool days_of_month[31];
    bool months[12];
    bool days_of_week[7];
    char *rule;
} cron_rule_t;

cron_rule_t * parse_config(char *filename);

bool rule_match(cron_rule_t rule, time_t time);

void compile_regex(regex_t *r, const char *regex_text);
bool match_regex(regex_t *r, const char *to_match);
void regex_init();
regex_t r_subtoken;
regex_t r_subtoken_num;
regex_t r_subtoken_range;
regex_t r_subtoken_div;
regex_t r_subtoken_range_div;
regex_t r_subtoken_str;

void parse_minutes(char *token, cron_rule_t *rule);
void parse_hours(char *token, cron_rule_t *rule);
void parse_days_of_month(char *token, cron_rule_t *rule);
void parse_months(char *token, cron_rule_t *rule);
void parse_days_of_week(char *token, cron_rule_t *rule);
bool* parse_subtoken(char *subtoken);

#endif // CRONPARSER_H_
