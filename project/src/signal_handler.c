#include "../include/shared.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

volatile sig_atomic_t terminate_requested = 0;

static void signal_handler(int signal)
{
    if (signal == SIGUSR1) {
        const char message[] =
            "SIGUSR1 received: User-defined event.\n";

        write(STDOUT_FILENO, message, sizeof(message) - 1);
    }
    else if (signal == SIGUSR2) {
        const char message[] =
            "SIGUSR2 received: Notification event.\n";

        write(STDOUT_FILENO, message, sizeof(message) - 1);
    }
    else if (signal == SIGTERM) {
        terminate_requested = 1;

        const char message[] =
            "SIGTERM received: Termination requested.\n";

        write(STDOUT_FILENO, message, sizeof(message) - 1);
    }
}

void setup_signal_handlers(void)
{
    struct sigaction sa;

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}
