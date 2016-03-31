/*
 * Copyright 2016 Nihlaeth
 */

#include <stdio.h>
#include <stdlib.h>
#include "../src/cronparser.h"
#include "../src/pickfile.h"

void main() {
    printf("\n\nPick_file integration test\n");
    printf("Failure is to be expected if your tmsu database was not initialized correctly.\n");
    regex_init();
    rule_container_t rules;
    rules.arr = malloc(sizeof(cron_rule_t));
    rules.num_rules = 0;
    parse_config(&rules, "./testpickfile.config");
    char *str = "";
    char **filename = &str;
    pick_file(&rules, filename);
    printf("my filename = |%s|\n", *filename);
    int i;
    for (i = 0; i < rules.num_rules; i++) {
        free(rules.arr[i].rule);
    }
    free(rules.arr);
    free(*filename);
    exit(EXIT_SUCCESS);
}
