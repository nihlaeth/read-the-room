/*
 * Copyright 2016 Nihlaeth
 */

#include "./cronparser.h"
#include "./pickfile.h"

void main() {
    regex_init();
    cron_rule_t ** rules = malloc(sizeof(cron_rule_t *));
    int rulesc = parse_config(rules, "./testconfig");
    char * filename = pick_file(rulesc, rules);
    printf("my filename = |%s|\n", filename);
    int i;
    for (i = 0; i < rulesc; i++) {
        free(rules[i]);
    }
    free(rules);
    free(filename);
}
