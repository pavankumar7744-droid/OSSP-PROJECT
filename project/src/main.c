#include "../include/shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

Semaphores sems;

int main(void)
{
    int shm_fd = create_shared_memory();

    SharedBuffer *shared =
        attach_shared_memory(shm_fd);

    initialize_shared_buffer(shared);

    sems = create_semaphores();

    setup_signal_handlers();

    printf(
        "Main Process | PID: %d\n",
        getpid()
    );

    fflush(stdout);

    // Create input.bin using Unbuffered I/O to cover CO-5
    int fd = open("input.bin", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd != -1) {
        for (int i = 1; i <= MAX_ITEMS; i++) {
            write(fd, &i, sizeof(int));
        }
        close(fd);
        printf("Main Process | Created input.bin with %d items (Unbuffered I/O)\n", MAX_ITEMS);
        fflush(stdout);
    } else {
        perror("open input.bin");
    }


    pid_t producer_pid = fork();

    if (producer_pid == -1) {
        perror("fork");
        cleanup_ipc(shared, shm_fd);
        exit(EXIT_FAILURE);
    }

    if (producer_pid == 0) {

        setup_signal_handlers();

        printf(
            "Producer Process | PID: %d | PPID: %d\n",
            getpid(),
            getppid()
        );

        fflush(stdout);

        producer_process(shared);

        exit(EXIT_SUCCESS);
    }

    pid_t consumer_pid = fork();

    if (consumer_pid == -1) {
        perror("fork");

        kill(producer_pid, SIGTERM);
        waitpid(producer_pid, NULL, 0);

        cleanup_ipc(shared, shm_fd);
        exit(EXIT_FAILURE);
    }

    if (consumer_pid == 0) {

        setup_signal_handlers();

        printf(
            "Consumer Process | PID: %d | PPID: %d\n",
            getpid(),
            getppid()
        );

        fflush(stdout);

        consumer_process(shared);

        exit(EXIT_SUCCESS);
    }

    printf(
        "Producer PID: %d\n",
        producer_pid
    );

    printf(
        "Consumer PID: %d\n",
        consumer_pid
    );

    fflush(stdout);

    waitpid(producer_pid, NULL, 0);
    waitpid(consumer_pid, NULL, 0);

    printf(
        "Both Producer and Consumer processes completed.\n"
    );

    cleanup_ipc(shared, shm_fd);

    return 0;
}
