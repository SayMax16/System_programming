#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define DEFAULT_PERMISSIONS 0644
#define MAX_MESSAGE_LENGTH 1024

// Structure to hold file metadata
typedef struct {
    time_t creation_time;
    mode_t permissions;
    size_t size;
} FileMetadata;

// Function to get current timestamp as string
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

// Function to create file with metadata
int create_file_with_metadata(const char *filename, const char *content, FileMetadata *metadata) {
    // Create file with specified permissions
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_PERMISSIONS);
    if (fd == -1) {
        fprintf(stderr, "Error creating file '%s': %s\n", filename, strerror(errno));
        return -1;
    }
    
    // Write content to file
    ssize_t bytes_written = write(fd, content, strlen(content));
    if (bytes_written == -1) {
        fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    
    // Get file metadata
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        fprintf(stderr, "Error getting file metadata: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    
    // Store metadata
    metadata->creation_time = file_stat.st_mtime;
    metadata->permissions = file_stat.st_mode & 0777;
    metadata->size = file_stat.st_size;
    
    // Close file
    if (close(fd) == -1) {
        fprintf(stderr, "Error closing file: %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

// Function to verify file content
int verify_file_content(const char *filename, const char *expected_content) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file for verification: %s\n", strerror(errno));
        return -1;
    }
    
    char buffer[MAX_MESSAGE_LENGTH];
    ssize_t bytes_read = read(fd, buffer, MAX_MESSAGE_LENGTH - 1);
    
    if (bytes_read == -1) {
        fprintf(stderr, "Error reading file: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    
    buffer[bytes_read] = '\0';
    
    int result = (strcmp(buffer, expected_content) == 0);
    
    if (close(fd) == -1) {
        fprintf(stderr, "Error closing file: %s\n", strerror(errno));
        return -1;
    }
    
    return result;
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "output.txt";
    const char *message = "Hello, this is a test message with timestamp: ";
    char timestamp[32];
    char full_message[MAX_MESSAGE_LENGTH];
    FileMetadata metadata;
    
    // Create message with timestamp
    get_timestamp(timestamp, sizeof(timestamp));
    snprintf(full_message, MAX_MESSAGE_LENGTH, "%s%s", message, timestamp);
    
    printf("Creating file '%s' with message:\n%s\n", filename, full_message);
    
    // Create file with metadata
    if (create_file_with_metadata(filename, full_message, &metadata) == -1) {
        return 1;
    }
    
    // Print file metadata
    printf("\nFile created successfully!\n");
    printf("File name: %s\n", filename);
    printf("Creation time: %s", ctime(&metadata.creation_time));
    printf("File permissions: %o\n", metadata.permissions);
    printf("File size: %zu bytes\n", metadata.size);
    
    // Verify file content
    printf("\nVerifying file content...\n");
    if (verify_file_content(filename, full_message)) {
        printf("Content verification successful!\n");
    } else {
        printf("Content verification failed!\n");
        return 1;
    }
    
    return 0;
} 