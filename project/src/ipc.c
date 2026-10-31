#include "../include/shared.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int create_shared_memory(void)
{
    int shm_fd = shm_open(
        SHM_NAME,
        O_CREAT | O_RDWR,
        0666
    );

    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(SharedBuffer)) == -1) {
        perror("ftruncate");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    return shm_fd;
}

SharedBuffer *attach_shared_memory(int shm_fd)
{
    SharedBuffer *shared = mmap(
        NULL,
        sizeof(SharedBuffer),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );

    if (shared == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        exit(EXIT_FAILURE);
    }

    return shared;
}

void initialize_shared_buffer(SharedBuffer *shared)
{
    shared->in = 0;
    shared->out = 0;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        shared->buffer[i] = 0;
    }
}

Semaphores create_semaphores(void)
{
    Semaphores sems_local;

    sem_unlink(EMPTY_SEM);
    sem_unlink(FULL_SEM);
    sem_unlink(MUTEX_SEM);

    sems_local.empty = sem_open(
        EMPTY_SEM,
        O_CREAT,
        0666,
        BUFFER_SIZE
    );

    sems_local.full = sem_open(
        FULL_SEM,
        O_CREAT,
        0666,
        0
    );

    sems_local.mutex = sem_open(
        MUTEX_SEM,
        O_CREAT,
        0666,
        1
    );

    if (sems_local.empty == SEM_FAILED ||
        sems_local.full == SEM_FAILED ||
        sems_local.mutex == SEM_FAILED) {

        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    return sems_local;
}

void cleanup_ipc(SharedBuffer *shared, int shm_fd)
{
    munmap(shared, sizeof(SharedBuffer));

    close(shm_fd);

    shm_unlink(SHM_NAME);

    sem_close(sems.empty);
    sem_close(sems.full);
    sem_close(sems.mutex);

    sem_unlink(EMPTY_SEM);
    sem_unlink(FULL_SEM);
    sem_unlink(MUTEX_SEM);
}
