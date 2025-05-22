#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_FILES 3

// Structure to hold file information
typedef struct {
    int fd;
    char path[256];
    int flags;
    mode_t mode;
} FileInfo;

// Function to safely close a file descriptor
void safe_close(int fd, const char *filename) {
    if (close(fd) == -1) {
        fprintf(stderr, "Error closing %s: %s\n", filename, strerror(errno));
    }
}

// Function to duplicate file descriptor with error handling
int safe_dup(int oldfd, const char *filename) {
    int newfd = dup(oldfd);
    if (newfd == -1) {
        fprintf(stderr, "Error duplicating %s: %s\n", filename, strerror(errno));
    }
    return newfd;
}

// Function to duplicate file descriptor to specific number with error handling
int safe_dup2(int oldfd, int newfd, const char *filename) {
    if (dup2(oldfd, newfd) == -1) {
        fprintf(stderr, "Error redirecting %s: %s\n", filename, strerror(errno));
        return -1;
    }
    return newfd;
}

// Function to read and display file content
void read_and_display(int fd, const char *filename) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    printf("\nReading from %s:\n", filename);
    printf("----------------------------------------\n");
    
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    
    if (bytes_read == -1) {
        fprintf(stderr, "Error reading from %s: %s\n", filename, strerror(errno));
    }
    
    printf("----------------------------------------\n");
}

// Function to write to file
void write_to_file(int fd, const char *filename, const char *content) {
    ssize_t bytes_written = write(fd, content, strlen(content));
    if (bytes_written == -1) {
        fprintf(stderr, "Error writing to %s: %s\n", filename, strerror(errno));
    } else if (bytes_written != strlen(content)) {
        fprintf(stderr, "Warning: Incomplete write to %s\n", filename);
    }
}

int main(int argc, char *argv[]) {
    FileInfo files[MAX_FILES] = {
        {-1, "input.txt", O_RDONLY, 0},
        {-1, "output1.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644},
        {-1, "output2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644}
    };
    
    // Open input file
    files[0].fd = open(files[0].path, files[0].flags);
    if (files[0].fd == -1) {
        perror("Error opening input file");
        return 1;
    }
    
    // Open output files
    for (int i = 1; i < MAX_FILES; i++) {
        files[i].fd = open(files[i].path, files[i].flags, files[i].mode);
        if (files[i].fd == -1) {
            perror("Error opening output file");
            // Clean up previously opened files
            for (int j = 0; j < i; j++) {
                safe_close(files[j].fd, files[j].path);
            }
            return 1;
        }
    }
    
    // Read and display original file content
    read_and_display(files[0].fd, files[0].path);
    
    // Duplicate file descriptor
    int dup_fd = safe_dup(files[0].fd, files[0].path);
    if (dup_fd != -1) {
        printf("\nUsing duplicated file descriptor:\n");
        read_and_display(dup_fd, files[0].path);
        safe_close(dup_fd, "duplicated descriptor");
    }
    
    // Redirect stdout to output1.txt
    int stdout_backup = safe_dup(STDOUT_FILENO, "stdout");
    if (stdout_backup != -1) {
        if (safe_dup2(files[1].fd, STDOUT_FILENO, files[1].path) != -1) {
            printf("This message goes to output1.txt\n");
            printf("File descriptor redirection test\n");
            
            // Restore stdout
            safe_dup2(stdout_backup, STDOUT_FILENO, "stdout");
            safe_close(stdout_backup, "stdout backup");
        }
    }
    
    // Write to output2.txt using original file descriptor
    write_to_file(files[2].fd, files[2].path, "This message goes to output2.txt\n");
    
    // Close all file descriptors
    for (int i = 0; i < MAX_FILES; i++) {
        safe_close(files[i].fd, files[i].path);
    }
    
    printf("\nFile operations completed. Check output1.txt and output2.txt for results.\n");
    
    return 0;
} 