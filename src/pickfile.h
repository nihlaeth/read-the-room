/*
 * Copyright 2016 Nihlaeth
 */

#ifndef SRC_PICKFILE_H_
#define SRC_PICKFILE_H_
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include "./cronparser.h"
#include "./memory.h"

void pick_file(rule_container_t *rules, char **file_name);

#endif // SRC_PICKFILE_H_
