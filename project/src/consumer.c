#include "../include/shared.h"

#include <stdio.h>
#include <unistd.h>

extern Semaphores sems;
extern volatile sig_atomic_t terminate_requested;

void consumer_process(SharedBuffer *shared)
{
    // Open output.txt using Buffered I/O (CO-5)
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Consumer: fopen output.txt");
        return;
    }

    for (int i = 1; i <= MAX_ITEMS; i++) {

        if (terminate_requested) {
            break;
        }

        sem_wait(sems.full);

        if (terminate_requested) {
            sem_post(sems.full);
            break;
        }

        sem_wait(sems.mutex);

        if (terminate_requested) {
            sem_post(sems.mutex);
            sem_post(sems.full);
            break;
        }

        int item = shared->buffer[shared->out];

        printf(
            "[Consumer | PID %d] Consumed: %d from position %d\n",
            getpid(),
            item,
            shared->out
        );
        // Write to file (Buffered I/O)
        fprintf(file, "Consumed: %d\n", item);
        fflush(file); // Ensure it's written

        fflush(stdout);

        shared->out =
            (shared->out + 1) % BUFFER_SIZE;

        sem_post(sems.mutex);

        sem_post(sems.empty);

        usleep(300000);
    }
    
    fclose(file);

    printf(
        "[Consumer | PID %d] Finished.\n",
        getpid()
    );

    fflush(stdout);
}
