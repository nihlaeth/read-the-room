/*
 * Copyright 2016 Nihlaeth
 */
#include <time.h>
#include <stdbool.h>
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
    test = match_regex(&r_subtoken, "Mon");
    printf("is Mon a subtoken? %d\n", test);
    test = match_regex(&r_subtoken_str, "Mon");
    printf("is Mon a string subtoken? %d\n", test);
    test = match_regex(&r_subtoken, "1-5/3");
    printf("is 1-5/3 a subtoken? %d\n", test);
    test = match_regex(&r_subtoken_range_div, "1-5/3");
    printf("is 1-5/3 a range division subtoken? %d\n", test);
    test = match_regex(&r_subtoken, "**");
    printf("is ** a subtoken? %d\n", test);


    /* test rule_match */
    cron_rule_t test_rule;
    init_rule(&test_rule);
    test_rule.minutes[0] = true;
    test_rule.days_of_month[26] = true;
    test_rule.months[2] = true;
    test_rule.days_of_week[0] = true;

    struct tm five_o_clock;
    five_o_clock.tm_sec = 0;
    five_o_clock.tm_min = 0;
    five_o_clock.tm_hour = 5;
    five_o_clock.tm_wday = 6;
    five_o_clock.tm_mday = 27;
    five_o_clock.tm_mon = 2;
    five_o_clock.tm_year = 116;
    five_o_clock.tm_isdst = 1;
    time_t five = mktime(&five_o_clock);

    bool test_result;
    test_result = rule_match(&test_rule, five);
    printf("rule matches 5:00? should be false, is %d\n", test_result);
    test_rule.hours[5] = true;
    test_result = rule_match(&test_rule, five);
    printf("rule matches 5:00? should be true, is %d\n", test_result);

    time_t current;
    time(&current);
    test_result = rule_match(&test_rule, current);
    printf("does current time match? should be false, is %d\n", test_result);

    exit(EXIT_SUCCESS);
}
