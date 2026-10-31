#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>

#define BUFFER_SIZE 5
#define MAX_ITEMS 20

#define SHM_NAME "/pc_shared_memory"

#define EMPTY_SEM "/pc_empty"
#define FULL_SEM "/pc_full"
#define MUTEX_SEM "/pc_mutex"

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;
    int out;
} SharedBuffer;

typedef struct {
    sem_t *empty;
    sem_t *full;
    sem_t *mutex;
} Semaphores;

extern Semaphores sems;

extern volatile sig_atomic_t terminate_requested;

int create_shared_memory(void);

SharedBuffer *attach_shared_memory(int shm_fd);

void initialize_shared_buffer(SharedBuffer *shared);

Semaphores create_semaphores(void);

void cleanup_ipc(SharedBuffer *shared, int shm_fd);

void producer_process(SharedBuffer *shared);

void consumer_process(SharedBuffer *shared);

void setup_signal_handlers(void);

#endif
