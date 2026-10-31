#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/pc_fifo"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf(
            "Usage: %s [read|write]\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (access(FIFO_NAME, F_OK) == -1) {
            perror("mkfifo");
            return EXIT_FAILURE;
        }
    }

    if (strcmp(argv[1], "write") == 0) {

        int fd = open(
            FIFO_NAME,
            O_WRONLY
        );

        if (fd == -1) {
            perror("open");
            return EXIT_FAILURE;
        }

        const char message[] =
            "Hello through named FIFO.";

        if (write(
            fd,
            message,
            sizeof(message)
        ) == -1) {
            perror("write");
            close(fd);
            return EXIT_FAILURE;
        }

        printf("FIFO writer sent: %s\n", message);

        close(fd);
    }

    else if (strcmp(argv[1], "read") == 0) {

        int fd = open(
            FIFO_NAME,
            O_RDONLY
        );

        if (fd == -1) {
            perror("open");
            return EXIT_FAILURE;
        }

        char message[100] = {0};

        ssize_t bytes_read =
            read(
                fd,
                message,
                sizeof(message) - 1
            );

        if (bytes_read == -1) {
            perror("read");
            close(fd);
            return EXIT_FAILURE;
        }

        message[bytes_read] = '\0';

        printf(
            "FIFO reader received: %s\n",
            message
        );

        close(fd);
    }

    else {
        printf(
            "Invalid option. Use 'read' or 'write'.\n"
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
