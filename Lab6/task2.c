#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH 256

// Structure to hold detailed file metadata
typedef struct {
    char path[MAX_PATH];
    struct stat stat_info;
    char type[20];
    char permissions[10];
    char last_modified[50];
    char last_accessed[50];
    char created[50];
} FileMetadata;

// Function to get file type string
void get_file_type(mode_t mode, char *type) {
    if (S_ISREG(mode)) strcpy(type, "Regular File");
    else if (S_ISDIR(mode)) strcpy(type, "Directory");
    else if (S_ISLNK(mode)) strcpy(type, "Symbolic Link");
    else if (S_ISFIFO(mode)) strcpy(type, "FIFO/Pipe");
    else if (S_ISSOCK(mode)) strcpy(type, "Socket");
    else if (S_ISBLK(mode)) strcpy(type, "Block Device");
    else if (S_ISCHR(mode)) strcpy(type, "Character Device");
    else strcpy(type, "Unknown");
}

// Function to get permissions string
void get_permissions(mode_t mode, char *permissions) {
    sprintf(permissions, "%c%c%c%c%c%c%c%c%c",
        (mode & S_IRUSR) ? 'r' : '-',
        (mode & S_IWUSR) ? 'w' : '-',
        (mode & S_IXUSR) ? 'x' : '-',
        (mode & S_IRGRP) ? 'r' : '-',
        (mode & S_IWGRP) ? 'w' : '-',
        (mode & S_IXGRP) ? 'x' : '-',
        (mode & S_IROTH) ? 'r' : '-',
        (mode & S_IWOTH) ? 'w' : '-',
        (mode & S_IXOTH) ? 'x' : '-'
    );
}

// Function to get formatted time string
void get_time_string(time_t time_val, char *time_str) {
    struct tm *tm_info = localtime(&time_val);
    strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Function to get file metadata
int get_file_metadata(const char *path, FileMetadata *metadata, int follow_links) {
    strncpy(metadata->path, path, MAX_PATH - 1);
    metadata->path[MAX_PATH - 1] = '\0';
    
    // Use stat or lstat based on follow_links parameter
    int result = follow_links ? stat(path, &metadata->stat_info) : lstat(path, &metadata->stat_info);
    if (result == -1) {
        return 0;
    }
    
    // Get file type
    get_file_type(metadata->stat_info.st_mode, metadata->type);
    
    // Get permissions
    get_permissions(metadata->stat_info.st_mode, metadata->permissions);
    
    // Get time information
    get_time_string(metadata->stat_info.st_mtime, metadata->last_modified);
    get_time_string(metadata->stat_info.st_atime, metadata->last_accessed);
    get_time_string(metadata->stat_info.st_ctime, metadata->created);
    
    return 1;
}

// Function to print file metadata
void print_metadata(const FileMetadata *metadata) {
    printf("\nFile: %s\n", metadata->path);
    printf("Type: %s\n", metadata->type);
    printf("Size: %lld bytes\n", (long long)metadata->stat_info.st_size);
    printf("Permissions: %s\n", metadata->permissions);
    printf("Inode: %llu\n", (unsigned long long)metadata->stat_info.st_ino);
    printf("Links: %hu\n", metadata->stat_info.st_nlink);
    printf("Owner: %u\n", metadata->stat_info.st_uid);
    printf("Group: %u\n", metadata->stat_info.st_gid);
    printf("Last Modified: %s\n", metadata->last_modified);
    printf("Last Accessed: %s\n", metadata->last_accessed);
    printf("Created: %s\n", metadata->created);
}

// Function to compare two file metadata
void compare_metadata(const FileMetadata *meta1, const FileMetadata *meta2) {
    printf("\nComparing %s and %s:\n", meta1->path, meta2->path);
    
    printf("Size: %s\n", 
        meta1->stat_info.st_size == meta2->stat_info.st_size ? "Same" : "Different");
    
    printf("Inode: %s\n", 
        meta1->stat_info.st_ino == meta2->stat_info.st_ino ? "Same" : "Different");
    
    printf("Type: %s\n", 
        strcmp(meta1->type, meta2->type) == 0 ? "Same" : "Different");
    
    printf("Permissions: %s\n", 
        strcmp(meta1->permissions, meta2->permissions) == 0 ? "Same" : "Different");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2]\n", argv[0]);
        return 1;
    }
    
    FileMetadata meta1, meta2;
    
    // Get metadata for first file using stat (follows links)
    if (!get_file_metadata(argv[1], &meta1, 1)) {
        perror("Error getting file metadata");
        return 1;
    }
    
    printf("=== File Metadata (using stat) ===\n");
    print_metadata(&meta1);
    
    // Get metadata for first file using lstat (doesn't follow links)
    if (!get_file_metadata(argv[1], &meta2, 0)) {
        perror("Error getting file metadata");
        return 1;
    }
    
    printf("\n=== File Metadata (using lstat) ===\n");
    print_metadata(&meta2);
    
    // If second file is provided, compare metadata
    if (argc > 2) {
        FileMetadata meta3;
        if (!get_file_metadata(argv[2], &meta3, 1)) {
            perror("Error getting second file metadata");
            return 1;
        }
        
        printf("\n=== Second File Metadata ===\n");
        print_metadata(&meta3);
        
        compare_metadata(&meta1, &meta3);
    }
    
    return 0;
} 