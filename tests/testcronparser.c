/*
 * Copyright 2016 Nihlaeth
 */
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "tap.h"
#include "../src/cronparser.h"

bool cmp_bool_array(bool* arr1, bool* arr2, int len, char* test_name) {
    int i;
    for (i = 0; i < len; i++) {
        if (arr1[i] != arr2[i]) {
            fail("%s", test_name);
            diag(
                "bool array comparison failed at position %d: %d != %d",
                i,
                arr1[i],
                arr2[i]);
            return false;
        }
    }
    return true;
}

void fill_bool_array(bool* arr, int len, bool def) {
    int i;
    for (i = 0; i < len; i++) {
        arr[i] = def;
    }
}

void main() {
    plan(27);
    
    /* compile regexes */
    lives_ok({
        regex_init();
    }, "compile regexes");

    /* test regexes */
    ok(match_regex(&r_subtoken, "10"), "is 10 a subtoken?");
    ok(match_regex(&r_subtoken_num, "10"), "is 10 a numeric subtoken?");
    ok(match_regex(&r_subtoken_div, "*/2"), "is */2 a division subtoken?");
    ok(match_regex(&r_subtoken, "Mon"), "is Mon a subtoken?");
    ok(match_regex(&r_subtoken_str, "jan"), "is jan a string subtoken?");
    ok(match_regex(&r_subtoken, "1-5/3"), "is 1-5/3 a subtoken?");
    ok(match_regex(&r_subtoken_range_div, "1-5/3"), "is 1-5/3 a range division subtoken?");
    ok(match_regex(&r_subtoken_num, "*"), "is * a subtoken?");
    ok(match_regex(&r_subtoken_range, "15-189"), "is 15-189 a range subtoken?");
    ok(!match_regex(&r_subtoken, "test"), "is test an invalid subtoken?");
    ok(!match_regex(&r_subtoken_range, "*-1"), "is * use in a range invalid?");
    ok(!match_regex(&r_subtoken_range_div, "1-5/12-15"), "is 1-5/12-15 an invalid range division subtoken?");
   

    /* test rule_match */
    cron_rule_t test_rule;
    init_rule(&test_rule);
    bool test_arr[60];
    fill_bool_array(test_arr, 60, false);
    if (cmp_bool_array(*test_rule.minutes, test_arr, 60, "does init_rule zero initialize the minutes array?")) {
        pass("does init_rule zero initialize the minutes array?");
    }

    (*test_rule.minutes)[0] = true;
    (*test_rule.days_of_month)[26] = true;
    (*test_rule.months)[2] = true;
    (*test_rule.days_of_week)[0] = true;

    struct tm test_time;
    /* Saturday 27 March 2016 05:00:00 */
    test_time.tm_sec = 0;
    test_time.tm_min = 0;
    test_time.tm_hour = 5;
    test_time.tm_wday = 6;
    test_time.tm_mday = 27;
    test_time.tm_mon = 2;
    test_time.tm_year = 116;
    test_time.tm_isdst = 1;
    time_t test_time_raw = mktime(&test_time);

    ok(!rule_match(&test_rule, test_time_raw), "do incompatible times fail to match?");
    (*test_rule.hours)[5] = true;
    ok(rule_match(&test_rule, test_time_raw), "do compatible times match?");


    /* test parse_subtoken */
    bool result[5];
    fill_bool_array(result, 5, false);

    bool expected[5];
    fill_bool_array(expected, 5, false);

    /* * */
    fill_bool_array(expected, 5, true);
    parse_subtoken(result, 5, "*");
    if (cmp_bool_array(result, expected, 5, "parse *")) {
        pass("parse *");
    }

    /* 4 */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[4] = true;
    parse_subtoken(result, 5, "4");
    if (cmp_bool_array(result, expected, 5, "parse 4")) {
        pass("parse 4");
    }

    /* 1-2 */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[1] = true;
    expected[2] = true;
    parse_subtoken(result, 5, "1-2");
    if (cmp_bool_array(result, expected, 5, "parse 1-2")) {
        pass("parse 1-2");
    }

    /* fEb */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[1] = true;
    parse_subtoken(result, 5 , "fEb");
    if (cmp_bool_array(result, expected, 5, "parse fEb")) {
        pass("parse fEb");
    }

    /* tue */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[2] = true;
    parse_subtoken(result, 5, "tue");
    if (cmp_bool_array(result, expected, 5, "parse tue")) {
        pass("parse tue");
    }

    /* * / 2 */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[2] = true;
    expected[4] = true;
    parse_subtoken(result, 5, "*/2");
    if (cmp_bool_array(result, expected, 5, "parse */2")) {
        pass("parse */2");
    }

    /* 1-3/2 */
    fill_bool_array(result, 5, false);
    fill_bool_array(expected, 5, false);
    expected[2] = true;
    parse_subtoken(result, 5, "1-3/2");
    if (cmp_bool_array(result, expected, 5, "parse 1-3/2")) {
        pass("parse 1-3/2");
    }

    /* test invalid subtoken */
    dies_ok({parse_subtoken(result, 5, "testing");}, "dies on invalid subtoken?");

    /* split_into_subtokens */
    bool hours_array[24];
    fill_bool_array(hours_array, 24, false);
    bool expected_hours[24];
    fill_bool_array(expected_hours, 24, false);
    expected_hours[1] = true;
    expected_hours[3] = true;
    expected_hours[8] = true;
    expected_hours[9] = true;
    expected_hours[10] = true;
    expected_hours[11] = true;
    expected_hours[12] = true;
    expected_hours[13] = true;
    expected_hours[14] = true;
    expected_hours[16] = true;
    char str[] = "1,3,8-14,16-21/8";
    split_into_subtokens(hours_array, 24, str);
    if (cmp_bool_array(hours_array, expected_hours, 24, "split into subtokens")) {
        pass("split into subtokens");
    }

    /* parse actual config file */
    int rulesc;
    cron_rule_t **rules = malloc(sizeof(cron_rule_t *));
    /*lives_ok({ */
        rulesc = parse_config(rules, "./testcronparser.config");
    /*}, "parse test config file");*/

    /* check that rule 0 checks out */
    // int rulesc = parse_config(rules, "./testcronparser.config");
    printf("check 0\n");
    printf("rules[0]->minutes[0] = %d\n", (*rules[0]->minutes)[0]);
    //printf("rules[0]->minutes = %d\n", rules[0]->minutes);
    bool expected_minutes[60];
    fill_bool_array(expected_minutes, 60, true);
    /*
    if (cmp_bool_array(
            rules[0]->minutes,
            expected_minutes,
            60,
            "introspect first rule -> minutes")) {
        pass("introspect first rule -> minutes");
    }
    */

    exit(exit_status());
}
