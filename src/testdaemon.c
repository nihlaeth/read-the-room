#include <syslog.h>
#include "daemonize.h"

void main() {
    daemonize("testdaemon");
    while (1) {
        sleep(2);
        syslog(LOG_INFO, "Daemon alive");
        break;
    }
}
