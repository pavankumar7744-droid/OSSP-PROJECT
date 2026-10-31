#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int pipe_fd[2];

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {

        close(pipe_fd[1]);

        char message[100] = {0};

        ssize_t bytes_read =
            read(
                pipe_fd[0],
                message,
                sizeof(message) - 1
            );

        if (bytes_read == -1) {
            perror("read");
            close(pipe_fd[0]);
            exit(EXIT_FAILURE);
        }

        message[bytes_read] = '\0';

        printf(
            "Child PID %d received: %s\n",
            getpid(),
            message
        );

        close(pipe_fd[0]);

        exit(EXIT_SUCCESS);
    }

    close(pipe_fd[0]);

    const char message[] =
        "Hello from parent through anonymous pipe.";

    if (write(
        pipe_fd[1],
        message,
        sizeof(message)
    ) == -1) {
        perror("write");
        close(pipe_fd[1]);
        return EXIT_FAILURE;
    }

    close(pipe_fd[1]);

    waitpid(pid, NULL, 0);

    printf("Parent: Child completed.\n");

    return EXIT_SUCCESS;
}
