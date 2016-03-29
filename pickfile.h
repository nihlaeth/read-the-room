/*
 * Copyright 2016 Nihlaeth
 */

#ifndef PICKFILE_H_
#define PICKFILE_H_
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include "./cronparser.h"

void pick_file(int rulesc, cron_rule_t **rules, char* file_name);

#endif // PICKFILE_H_
