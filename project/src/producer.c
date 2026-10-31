#include "../include/shared.h"

#include <stdio.h>
#include <unistd.h>

extern Semaphores sems;
extern volatile sig_atomic_t terminate_requested;

void producer_process(SharedBuffer *shared)
{
    // Open input.bin using Unbuffered I/O (CO-5)
    int fd = open("input.bin", O_RDONLY);
    if (fd == -1) {
        perror("Producer: open input.bin");
        return;
    }

    int item;
    while (read(fd, &item, sizeof(int)) == sizeof(int)) {

        if (terminate_requested) {
            break;
        }

        sem_wait(sems.empty);

        if (terminate_requested) {
            sem_post(sems.empty);
            break;
        }

        sem_wait(sems.mutex);

        if (terminate_requested) {
            sem_post(sems.mutex);
            sem_post(sems.empty);
            break;
        }

        shared->buffer[shared->in] = item;

        printf(
            "[Producer | PID %d] Produced (from file): %d at position %d\n",
            getpid(),
            item,
            shared->in
        );

        fflush(stdout);

        shared->in =
            (shared->in + 1) % BUFFER_SIZE;

        sem_post(sems.mutex);

        sem_post(sems.full);

        usleep(200000);
    }
    close(fd);

    printf(
        "[Producer | PID %d] Finished.\n",
        getpid()
    );

    fflush(stdout);
}
