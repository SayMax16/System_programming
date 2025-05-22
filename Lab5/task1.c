#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

// Function to check if file exists
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to get detailed error message
void print_error_details(const char *operation, const char *filename) {
    fprintf(stderr, "Error during %s of file '%s':\n", operation, filename);
    fprintf(stderr, "Error code: %d\n", errno);
    fprintf(stderr, "Error message: %s\n", strerror(errno));
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "sample.txt";
    
    // Check if file exists before attempting to open
    if (!file_exists(filename)) {
        fprintf(stderr, "File '%s' does not exist.\n", filename);
        return 1;
    }
    
    // Try to open the file
    int fd = open(filename, O_RDONLY);
    
    if (fd == -1) {
        print_error_details("opening", filename);
        return 1;
    }
    
    // Get file information
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        print_error_details("getting file information", filename);
        close(fd);
        return 1;
    }
    
    // Print file details
    printf("File opened successfully!\n");
    printf("File name: %s\n", filename);
    printf("File size: %ld bytes\n", (long)file_stat.st_size);
    printf("File permissions: %o\n", file_stat.st_mode & 0777);
    
    // Close the file
    if (close(fd) == -1) {
        print_error_details("closing", filename);
        return 1;
    }
    
    printf("File closed successfully.\n");
    return 0;
} 