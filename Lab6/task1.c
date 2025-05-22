#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_OFFSETS 5

// Structure to hold offset information
typedef struct {
    off_t offset;
    size_t length;
    char description[50];
} OffsetInfo;

// Function to verify file content at specific offset
int verify_content(int fd, off_t offset, size_t length, const char *expected) {
    char buffer[BUFFER_SIZE];
    
    // Move to specified offset
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking in file");
        return 0;
    }
    
    // Read content
    ssize_t bytes_read = read(fd, buffer, length);
    if (bytes_read == -1) {
        perror("Error reading file");
        return 0;
    }
    
    buffer[bytes_read] = '\0';
    
    // Compare with expected content
    return (strcmp(buffer, expected) == 0);
}

// Function to print file content at specific offset
void print_content_at_offset(int fd, off_t offset, size_t length) {
    char buffer[BUFFER_SIZE];
    
    // Move to specified offset
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking in file");
        return;
    }
    
    // Read and print content
    ssize_t bytes_read = read(fd, buffer, length);
    if (bytes_read == -1) {
        perror("Error reading file");
        return;
    }
    
    buffer[bytes_read] = '\0';
    printf("Content at offset %ld: %s\n", (long)offset, buffer);
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "sample.txt";
    
    // Open file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    
    // Get file size
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Error getting file information");
        close(fd);
        return 1;
    }
    
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", (long)file_stat.st_size);
    
    // Define offsets to check
    OffsetInfo offsets[] = {
        {0, 10, "Start of file"},
        {5, 15, "Middle section"},
        {file_stat.st_size - 10, 10, "End of file"},
        {20, 20, "Custom section"},
        {40, 15, "Final section"}
    };
    
    // Check each offset
    for (int i = 0; i < MAX_OFFSETS; i++) {
        printf("\nChecking %s:\n", offsets[i].description);
        print_content_at_offset(fd, offsets[i].offset, offsets[i].length);
        
        // Verify content if expected value is provided
        if (argc > 2) {
            const char *expected = argv[2];
            if (verify_content(fd, offsets[i].offset, offsets[i].length, expected)) {
                printf("Content verification successful!\n");
            } else {
                printf("Content verification failed!\n");
            }
        }
    }
    
    // Close file
    if (close(fd) == -1) {
        perror("Error closing file");
        return 1;
    }
    
    return 0;
} 