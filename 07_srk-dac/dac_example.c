#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main() {
    const char *filename = "dac_test_file.txt";
    int fd;

    // Create a file with read and write permissions for the owner
    fd = open(filename, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
    }
    close(fd);

    // Change file permissions to read-only for the owner
    if (chmod(filename, S_IRUSR) == -1) {
        perror("Failed to change file permissions");
        exit(EXIT_FAILURE);
    }

    // Attempt to write to the file (should fail)
    fd = open(filename, O_WRONLY);
    if (fd == -1) {
        perror("Write attempt failed as expected");
    } else {
        printf("Unexpectedly succeeded in opening file for writing\n");
        close(fd);
    }

    // Change file permissions to read and write for the owner
    if (chmod(filename, S_IRUSR | S_IWUSR) == -1) {
        perror("Failed to change file permissions");
        exit(EXIT_FAILURE);
    }

    // Attempt to write to the file again (should succeed)
    fd = open(filename, O_WRONLY);
    if (fd == -1) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    } else {
        printf("Successfully opened file for writing\n");
        close(fd);
    }

    // Clean up
    if (remove(filename) == -1) {
        perror("Failed to remove file");
        exit(EXIT_FAILURE);
    }

    return 0;
}
