#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define MAX_LINE_LENGTH 256

// Structure to hold file statistics
typedef struct {
    int total_lines;
    int total_chars;
    int max_line_length;
    char longest_line[MAX_LINE_LENGTH];
} FileStats;

// Function to initialize file statistics
void init_stats(FileStats *stats) {
    stats->total_lines = 0;
    stats->total_chars = 0;
    stats->max_line_length = 0;
    memset(stats->longest_line, 0, MAX_LINE_LENGTH);
}

// Function to update statistics with a new line
void update_stats(FileStats *stats, const char *line, int length) {
    stats->total_lines++;
    stats->total_chars += length;
    
    if (length > stats->max_line_length) {
        stats->max_line_length = length;
        strncpy(stats->longest_line, line, MAX_LINE_LENGTH - 1);
        stats->longest_line[MAX_LINE_LENGTH - 1] = '\0';
    }
}

// Function to print file statistics
void print_stats(const FileStats *stats) {
    printf("\nFile Statistics:\n");
    printf("Total lines: %d\n", stats->total_lines);
    printf("Total characters: %d\n", stats->total_chars);
    printf("Maximum line length: %d\n", stats->max_line_length);
    printf("Longest line: %s\n", stats->longest_line);
}

int main(int argc, char *argv[]) {
    const char *filename = (argc > 1) ? argv[1] : "sample.txt";
    int fd = open(filename, O_RDONLY);
    
    if (fd == -1) {
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        return 1;
    }
    
    char buffer[BUFFER_SIZE];
    char line[MAX_LINE_LENGTH];
    int line_pos = 0;
    FileStats stats;
    init_stats(&stats);
    
    printf("Reading file: %s\n", filename);
    printf("----------------------------------------\n");
    
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] == '\n' || buffer[i] == '\0') {
                line[line_pos] = '\0';
                if (line_pos > 0) {
                    printf("Line %d: %s\n", stats.total_lines + 1, line);
                    update_stats(&stats, line, line_pos);
                }
                line_pos = 0;
            } else if (line_pos < MAX_LINE_LENGTH - 1) {
                line[line_pos++] = buffer[i];
            }
        }
    }
    
    // Handle the last line if it doesn't end with newline
    if (line_pos > 0) {
        line[line_pos] = '\0';
        printf("Line %d: %s\n", stats.total_lines + 1, line);
        update_stats(&stats, line, line_pos);
    }
    
    if (bytes_read == -1) {
        fprintf(stderr, "Error reading file: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    
    printf("----------------------------------------\n");
    print_stats(&stats);
    
    if (close(fd) == -1) {
        fprintf(stderr, "Error closing file: %s\n", strerror(errno));
        return 1;
    }
    
    return 0;
} 