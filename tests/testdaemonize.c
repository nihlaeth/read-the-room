/*
 * Copyright 2016 Nihlaeth
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tap.h"
#include "../src/daemonize.h"

void main() {
    plan(1);
    /* FIXME:
     * pretty useless test... it will succeed if it failed or not
     * maybe one day I'll check the syslog for the daemon started
     * message, or open up some sort of pipe, but that's too much
     * effort right now.
     */
    dies_ok({
        daemonize("test-daemon");
    }, "the demon's alive!");
    exit(exit_status());
}
