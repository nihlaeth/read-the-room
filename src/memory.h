/*
 * Copyright 2016 Nihlaeth
 */
#ifndef SRC_MEMORY_H_
#define SRC_MEMORY_H_

#include <stdlib.h>
#include <syslog.h>

void *xmalloc(size_t size);
void *xcalloc(size_t size, size_t item_size);
void *xrealloc(void *p, size_t size);

#endif // SRC_MEMORY_H_
