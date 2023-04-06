#Name: Tram Trinh (htrinh@nd.edu)
#Project 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

long bytes_count = 0;

void file_copy(char *program, char *source, char *target) {
    /* Open source file */
    int source_fd = open(source, O_RDONLY, 0);
    if (source_fd < 0) {
        printf("%s: counldn't open %s: %s\n", program, source, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Check if target file already exists */
    if (access(target, F_OK) == 0) {
        printf("%s: %s already exists.\n", program, target);
        exit(EXIT_FAILURE);
    }

    /* Create target file */
    int target_fd = creat(target, S_IRWXU);
    if (target_fd < 0) {
        printf("%s: counldn't create %s: %s\n", program, target, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Copy file from source to target */
    char buffer[4096];
    int nread, nwrite;

    while ((nread = read(source_fd, buffer, sizeof(buffer))) > 0) {
        // Write to file
        nwrite = write(target_fd, buffer, nread);
        // Check if write is valid 
        if (nwrite < 0) {
            if (errno != EINTR) { // If not Interupted system call
                printf("%s: couldn't write to file %s: %s\n", program, target, strerror(errno));
                exit(EXIT_FAILURE);
            }
        }

        // Write the remaining to file
        while (nread != nwrite) {
            int remain = write(target_fd, buffer + nwrite, nread - nwrite);
             // Check if write is valid 
            if (remain < 0) {
                if (errno != EINTR) { // If not Interupted system call
                    printf("%s: write to file %s partially succeed: %s\n", program, target, strerror(errno));
                    exit(EXIT_FAILURE);
            }
            nwrite += remain;
            }
        }
        bytes_count += nread;
    }

    /* Close source and target files */
    close(source_fd);
    close(target_fd);

    /* File copy succeeded */
    printf("%s: copied %ld bytes from %s to %s.\n", program, bytes_count, source, target);
}

int main(int argc, char *argv[]) {
    /* Parse command line options */
    if (argc < 3) {
        printf("usage: %s <sourcefile> <targetfile>\n", argv[0]);
        return EXIT_FAILURE;
    } 
    else if (argc > 3) {
        printf("%s: Too many arguments!", argv[0]);
        printf("usage: %s <sourcefile> <targetfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Copy file from source to target */
    file_copy(argv[0], argv[1], argv[2]);

    return EXIT_SUCCESS;
}
