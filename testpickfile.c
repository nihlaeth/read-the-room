/*
 * Copyright 2016 Nihlaeth
 */

#include "./cronparser.h"
#include "./pickfile.h"

void main() {
    printf("test check 0\n");
    regex_init();
    cron_rule_t ** rules = malloc(sizeof(cron_rule_t *));
    int rulesc = parse_config(rules, "./testconfig");
    printf("test check 1\n");
    char * filename = pick_file(rulesc, rules);
    printf("my filename = |%s|\n", filename);
    free(rules);
    free(filename);
}
