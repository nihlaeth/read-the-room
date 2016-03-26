/*
 * Copyright 2016 Nihlaeth
 */

#ifndef CRONPARSER_H_
#define CRONPARSER_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct cron_rule_t {
    bool minutes[60];
    bool hours[24];
    bool days_of_month[31];
    bool month[12];
    bool days_of_week[7];
    char *rule;
} cron_rule_t;

cron_rule_t * parse_config(char *filename);

bool rule_match(cron_rule_t rule, time_t time);

#endif // CRONPARSER_H_
