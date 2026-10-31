#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t terminate_requested = 0;

void signal_handler(int signal)
{
    if (signal == SIGUSR1) {

        const char message[] =
            "SIGUSR1 received: User-defined event.\n";

        write(
            STDOUT_FILENO,
            message,
            sizeof(message) - 1
        );
    }

    else if (signal == SIGUSR2) {

        const char message[] =
            "SIGUSR2 received: Notification event.\n";

        write(
            STDOUT_FILENO,
            message,
            sizeof(message) - 1
        );
    }

    else if (signal == SIGTERM) {

        terminate_requested = 1;

        const char message[] =
            "SIGTERM received: Termination requested.\n";

        write(
            STDOUT_FILENO,
            message,
            sizeof(message) - 1
        );
    }
}

int main(void)
{
    struct sigaction sa;

    sa.sa_handler = signal_handler;

    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    printf(
        "Signal Demo PID: %d\n",
        getpid()
    );

    printf(
        "Use SIGUSR1, SIGUSR2 or SIGTERM from another terminal.\n"
    );

    fflush(stdout);

    while (!terminate_requested) {
        pause();
    }

    printf("Signal demo terminated.\n");

    return EXIT_SUCCESS;
}
