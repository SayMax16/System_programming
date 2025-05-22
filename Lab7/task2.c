#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 4096
#define PROGRESS_BAR_WIDTH 50

// Structure to hold file copy progress
typedef struct {
    off_t total_bytes;
    off_t copied_bytes;
    time_t start_time;
    unsigned long checksum;
} CopyProgress;

// Function to initialize progress tracking
void init_progress(CopyProgress *progress, off_t total_size) {
    progress->total_bytes = total_size;
    progress->copied_bytes = 0;
    progress->start_time = time(NULL);
    progress->checksum = 0;
}

// Function to update checksum
void update_checksum(CopyProgress *progress, const char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        progress->checksum = (progress->checksum << 1) ^ buffer[i];
    }
}

// Function to display progress bar
void display_progress(const CopyProgress *progress) {
    float percentage = (float)progress->copied_bytes / progress->total_bytes;
    int filled = (int)(percentage * PROGRESS_BAR_WIDTH);
    time_t current_time = time(NULL);
    double elapsed = difftime(current_time, progress->start_time);
    double speed = elapsed > 0 ? progress->copied_bytes / elapsed : 0;
    
    printf("\r[");
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %.1f%% (%.2f MB/s)", percentage * 100, speed / (1024 * 1024));
    fflush(stdout);
}

// Function to get file size
off_t get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error getting file size");
        return -1;
    }
    return st.st_size;
}

// Function to create symbolic link with error handling
int create_symlink(const char *target, const char *linkpath) {
    if (symlink(target, linkpath) == -1) {
        perror("Error creating symbolic link");
        return -1;
    }
    return 0;
}

// Function to display file metadata
void display_file_metadata(const char *filename) {
    struct stat st;
    if (lstat(filename, &st) == -1) {
        perror("Error getting file metadata");
        return;
    }
    
    printf("\nFile Metadata for %s:\n", filename);
    printf("Size: %lld bytes\n", (long long)st.st_size);
    printf("Inode: %llu\n", (unsigned long long)st.st_ino);
    printf("Permissions: %o\n", st.st_mode & 0777);
    printf("Links: %hu\n", st.st_nlink);
    printf("Owner: %u\n", st.st_uid);
    printf("Group: %u\n", st.st_gid);
    
    printf("File Type: ");
    if (S_ISREG(st.st_mode)) printf("Regular File\n");
    else if (S_ISDIR(st.st_mode)) printf("Directory\n");
    else if (S_ISLNK(st.st_mode)) printf("Symbolic Link\n");
    else if (S_ISBLK(st.st_mode)) printf("Block Device\n");
    else if (S_ISCHR(st.st_mode)) printf("Character Device\n");
    else if (S_ISFIFO(st.st_mode)) printf("FIFO/Pipe\n");
    else if (S_ISSOCK(st.st_mode)) printf("Socket\n");
    else printf("Unknown\n");
    
    printf("Last Modified: %s", ctime(&st.st_mtime));
    printf("Last Accessed: %s", ctime(&st.st_atime));
    printf("Created: %s", ctime(&st.st_ctime));
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }
    
    // Get source file size
    off_t file_size = get_file_size(argv[1]);
    if (file_size == -1) {
        return 1;
    }
    
    // Open source file
    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return 1;
    }
    
    // Open destination file
    int dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        return 1;
    }
    
    // Initialize progress tracking
    CopyProgress progress;
    init_progress(&progress, file_size);
    
    // Copy file with progress tracking
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    
    printf("Copying %s to %s...\n", argv[1], argv[2]);
    
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dst_fd);
            return 1;
        }
        
        progress.copied_bytes += bytes_written;
        update_checksum(&progress, buffer, bytes_written);
        display_progress(&progress);
    }
    
    printf("\n");
    
    if (bytes_read == -1) {
        perror("Error reading from source file");
        close(src_fd);
        close(dst_fd);
        return 1;
    }
    
    close(src_fd);
    close(dst_fd);
    
    printf("\nFile copied successfully!\n");
    printf("Total bytes: %lld\n", (long long)progress.copied_bytes);
    printf("Checksum: %lu\n", progress.checksum);
    
    // Create symbolic link
    if (create_symlink(argv[2], "dest_link") == 0) {
        printf("\nSymbolic link 'dest_link' created successfully.\n");
        display_file_metadata("dest_link");
    }
    
    return 0;
} 