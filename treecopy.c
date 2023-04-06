#Name: Tram Trinh (htrinh@nd.edu)
#Project 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int dirs_count = 0;
int files_count = 0;
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
    printf("%s -> %s\n", source, target);
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
    }

    /* Close source and target files */
    close(source_fd);
    close(target_fd);

}

void tree_copy(char *program, char *source, char *target) {
    struct stat buf; // pointer to area which information should be written
    
    /* Open source directory */
    DIR *source_dir = opendir(source);
    if (!source_dir) {
        printf("%s: couldn't open %s: %s\n", program, source, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Check if target directory already exist */
    if (strcmp(target, ".") != 0 && strcmp(target, "..") != 0 && strcmp(target, "~") != 0) {
        DIR *target_dir = opendir(target);
        if (target_dir) { // directory exists
            printf("%s: %s already exits.\n", program, target);
            closedir(target_dir);
            exit(EXIT_FAILURE);
        }
        else if (ENOENT == errno) { // directory does not exist
            /* Create target directory */
            printf("%s -> %s\n", source, target);
            mkdir(target, S_IRWXU);
            dirs_count++;
        }
        else { // opendir failed
            printf("%s: %s directory is not valid: %s\n", program, target, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Create target path from source path */
    char source_path[BUFSIZ];
    char target_path[BUFSIZ];
    
    // Iterate through the source directory
    for (struct dirent *entry = readdir(source_dir); entry; entry = readdir(source_dir)) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Create target path from source path
        sprintf(source_path, "%s/%s", source, entry->d_name);
        sprintf(target_path, "%s/%s", target, entry->d_name);

        // Check if current source path is valid
        if (stat(source_path, &buf) < 0) {
            printf("%s: couldn't open directory %s: %s\n", program, source_path, strerror(errno));
            exit(EXIT_FAILURE);
        }

        // Check if the current source path is a directory
        if (S_ISDIR(buf.st_mode)) {
            // Recursive case
            tree_copy(program, source_path, target_path);
        }

        // If not directory, check if it is a file
        else if (S_ISREG(buf.st_mode)) {
            files_count++;
            bytes_count += buf.st_size;
            file_copy(program, source_path, target_path);
        }

        // Neither a directory nor a file
        else {
            printf("%s: %s is neither a directory nor a file\n", program, source_path);
            exit(EXIT_FAILURE);
        }
    }

    /* Close source directory */
    closedir(source_dir);  
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
    
    struct stat buf;
    /* Check if source is directory or file */
    if (stat(argv[1], &buf) < 0) {
            printf("%s: couldn't open directory %s: %s\n", argv[0], argv[1], strerror(errno));
            return EXIT_FAILURE;
    }
    if (S_ISDIR(buf.st_mode))
        tree_copy(argv[0], argv[1], argv[2]);
    else if (S_ISREG(buf.st_mode)) {
        files_count++;
        bytes_count += buf.st_size;
        file_copy(argv[0], argv[1], argv[2]);
    }

    /* Tree copying succeeded */  
    printf("%s: copied %d directories, %d files, and %ld bytes from %s to %s\n", argv[0], dirs_count, files_count, bytes_count, argv[1], argv[2]);
    return EXIT_SUCCESS;
}
