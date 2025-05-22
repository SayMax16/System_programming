#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define BUFFER_SIZE 4096
#define PROGRESS_BAR_WIDTH 50

// Structure to hold copy progress
typedef struct {
    size_t total_bytes;
    size_t copied_bytes;
    time_t start_time;
    unsigned long checksum;
} CopyProgress;

// Function to initialize copy progress
void init_progress(CopyProgress *progress, size_t total_size) {
    progress->total_bytes = total_size;
    progress->copied_bytes = 0;
    progress->start_time = time(NULL);
    progress->checksum = 0;
}

// Function to update checksum
void update_checksum(CopyProgress *progress, const char *buffer, size_t length) {
    for (size_t i = 0; i < length; i++) {
        progress->checksum = (progress->checksum << 1) ^ buffer[i];
    }
}

// Function to display progress bar
void display_progress(const CopyProgress *progress) {
    float percentage = (float)progress->copied_bytes / progress->total_bytes;
    int filled = (int)(percentage * PROGRESS_BAR_WIDTH);
    
    printf("\r[");
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %.1f%%", percentage * 100);
    
    // Calculate and display speed
    time_t current_time = time(NULL);
    double elapsed = difftime(current_time, progress->start_time);
    if (elapsed > 0) {
        double speed = progress->copied_bytes / elapsed;
        printf(" (%.1f KB/s)", speed / 1024);
    }
    
    fflush(stdout);
}

// Function to get file size
size_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        return 0;
    }
    return st.st_size;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }
    
    const char *source_file = argv[1];
    const char *dest_file = argv[2];
    
    // Open source file
    int src_fd = open(source_file, O_RDONLY);
    if (src_fd == -1) {
        fprintf(stderr, "Error opening source file '%s': %s\n", source_file, strerror(errno));
        return 1;
    }
    
    // Get source file size
    size_t file_size = get_file_size(source_file);
    if (file_size == 0) {
        fprintf(stderr, "Error: Source file is empty or cannot be accessed\n");
        close(src_fd);
        return 1;
    }
    
    // Open destination file
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        fprintf(stderr, "Error opening destination file '%s': %s\n", dest_file, strerror(errno));
        close(src_fd);
        return 1;
    }
    
    // Initialize progress tracking
    CopyProgress progress;
    init_progress(&progress, file_size);
    
    printf("Copying '%s' to '%s' (%zu bytes)\n", source_file, dest_file, file_size);
    
    // Copy file with progress tracking
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
        
        if (bytes_written == -1) {
            fprintf(stderr, "\nError writing to destination file: %s\n", strerror(errno));
            close(src_fd);
            close(dest_fd);
            return 1;
        }
        
        if (bytes_written != bytes_read) {
            fprintf(stderr, "\nError: Incomplete write operation\n");
            close(src_fd);
            close(dest_fd);
            return 1;
        }
        
        // Update progress
        progress.copied_bytes += bytes_written;
        update_checksum(&progress, buffer, bytes_written);
        display_progress(&progress);
    }
    
    if (bytes_read == -1) {
        fprintf(stderr, "\nError reading from source file: %s\n", strerror(errno));
        close(src_fd);
        close(dest_fd);
        return 1;
    }
    
    printf("\n\nFile copy completed successfully!\n");
    printf("Total bytes copied: %zu\n", progress.copied_bytes);
    printf("Checksum: %lu\n", progress.checksum);
    
    // Close files
    if (close(src_fd) == -1 || close(dest_fd) == -1) {
        fprintf(stderr, "Error closing files: %s\n", strerror(errno));
        return 1;
    }
    
    return 0;
} 