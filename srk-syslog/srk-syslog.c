#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main() {
    // Open a connection to the syslog
    openlog("my_program", LOG_PID | LOG_CONS, LOG_USER);

    // Log a message with different severity levels
    syslog(LOG_INFO, "This is an informational message.");
    syslog(LOG_WARNING, "This is a warning message.");
    syslog(LOG_ERR, "This is an error message.");

    // Close the connection to the syslog
    closelog();

    return 0;
}