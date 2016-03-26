/*
 * Copyright 2016 Nihlaeth
 */
#include "cronparser.h"

void main() {
    regex_init();
    cron_rule_t * rules = parse_config("./testconfig");


    /* test regex stuff */
    bool test;
    test = match_regex(&r_subtoken, "10");
    printf("is 10 a subtoken? %d\n", test);
    test = match_regex(&r_subtoken_num, "10");
    printf("is 10 a numeric subtoken? %d\n", test);
    test = match_regex(&r_subtoken_div, "*/2");
    printf("is */2 a division subtoken? %d\n", test);
    test = match_regex(&r_subtoken_str, "Mon");
    printf("is Mon a string subtoken? %d\n", test);
    test = match_regex(&r_subtoken, "1-5/3");
    printf("is 1-5/3 a subtoken? %d\n", test);
    test = match_regex(&r_subtoken_range_div, "1-5/3");
    printf("is 1-5/3 a range division subtoken? %d\n", test);
    test = match_regex(&r_subtoken, "**");
    printf("is ** a subtoken? %d\n", test);
    exit(EXIT_SUCCESS);
}
