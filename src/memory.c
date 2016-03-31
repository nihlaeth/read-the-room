/*
 * Copyright 2016 Nihlaeth
 */

#include <stdlib.h>
#include <syslog.h>
#include "./memory.h"

void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        syslog(LOG_ERR, "malloc() failed to allocate %d bytes", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

void *xcalloc(size_t size, size_t item_size) {
    void *p = calloc(size, item_size);
    if (p == NULL) {
        syslog(LOG_ERR, "calloc() failed to allocate %d bytes", size);
        exit(EXIT_FAILURE);
    }
    return p;
}

void *xrealloc(void *p, size_t size) {
    p = realloc(p, size);
    if (p == NULL) {
        syslog(LOG_ERR, "realloc() failed to allocate %d bytes", size);
        exit(EXIT_FAILURE);
    }
    return p;
}
