/*
 * Copyright 2016 Nihlaeth
 */
#include "cronparser.h"

void main() {
    cron_rule_t * rules = parse_config("./testconfig");

    exit(EXIT_SUCCESS);
}
