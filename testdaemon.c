#include <syslog.h>
#include "daemonize.h"

void main() {
    daemonize();
    while (1) {
        sleep(2);
        syslog(LOG_INFO, "Daemon alive");
        break;
    }
}
