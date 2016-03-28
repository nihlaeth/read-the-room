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

char* pick_file(int rulesc, cron_rule_t **rules);

#endif // PICKFILE_H_
