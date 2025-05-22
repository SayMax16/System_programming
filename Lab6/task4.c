#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH 256
#define MAX_FORMAT 1024
#define MAX_LINE 2048

// Structure to hold formatted metadata
typedef struct {
    char path[MAX_PATH];
    char size_str[50];
    char inode_str[50];
    char time_str[50];
    char permissions_str[50];
    char type_str[50];
    char owner_str[50];
    char group_str[50];
} FormattedMetadata;

// Function to format file size with appropriate units
void format_size(off_t size, char *size_str) {
    const char *units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size_double = size;
    
    while (size_double >= 1024 && unit_index < 4) {
        size_double /= 1024;
        unit_index++;
    }
    
    sprintf(size_str, "%.2f %s", size_double, units[unit_index]);
}

// Function to format time with timezone
void format_time(time_t time_val, char *time_str) {
    struct tm *tm_info = localtime(&time_val);
    char timezone[10];
    strftime(timezone, sizeof(timezone), "%z", tm_info);
    strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
    strcat(time_str, " ");
    strcat(time_str, timezone);
}

// Function to format permissions
void format_permissions(mode_t mode, char *perm_str) {
    sprintf(perm_str, "%c%c%c%c%c%c%c%c%c (%o)",
        (mode & S_IRUSR) ? 'r' : '-',
        (mode & S_IWUSR) ? 'w' : '-',
        (mode & S_IXUSR) ? 'x' : '-',
        (mode & S_IRGRP) ? 'r' : '-',
        (mode & S_IWGRP) ? 'w' : '-',
        (mode & S_IXGRP) ? 'x' : '-',
        (mode & S_IROTH) ? 'r' : '-',
        (mode & S_IWOTH) ? 'w' : '-',
        (mode & S_IXOTH) ? 'x' : '-',
        mode & 0777
    );
}

// Function to get file type string
void get_file_type(mode_t mode, char *type_str) {
    if (S_ISREG(mode)) strcpy(type_str, "Regular File");
    else if (S_ISDIR(mode)) strcpy(type_str, "Directory");
    else if (S_ISLNK(mode)) strcpy(type_str, "Symbolic Link");
    else if (S_ISFIFO(mode)) strcpy(type_str, "FIFO/Pipe");
    else if (S_ISSOCK(mode)) strcpy(type_str, "Socket");
    else if (S_ISBLK(mode)) strcpy(type_str, "Block Device");
    else if (S_ISCHR(mode)) strcpy(type_str, "Character Device");
    else strcpy(type_str, "Unknown");
}

// Function to format metadata
void format_metadata(const char *path, FormattedMetadata *fmt_meta) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Error getting file metadata");
        return;
    }
    
    strncpy(fmt_meta->path, path, MAX_PATH - 1);
    fmt_meta->path[MAX_PATH - 1] = '\0';
    
    format_size(st.st_size, fmt_meta->size_str);
    sprintf(fmt_meta->inode_str, "%llu", (unsigned long long)st.st_ino);
    format_time(st.st_mtime, fmt_meta->time_str);
    format_permissions(st.st_mode, fmt_meta->permissions_str);
    get_file_type(st.st_mode, fmt_meta->type_str);
    sprintf(fmt_meta->owner_str, "%u", st.st_uid);
    sprintf(fmt_meta->group_str, "%u", st.st_gid);
}

// Function to write formatted metadata to file
void write_metadata(FILE *file, const FormattedMetadata *fmt_meta) {
    fprintf(file, "File: %s\n", fmt_meta->path);
    fprintf(file, "Type: %s\n", fmt_meta->type_str);
    fprintf(file, "Size: %s\n", fmt_meta->size_str);
    fprintf(file, "Inode: %s\n", fmt_meta->inode_str);
    fprintf(file, "Permissions: %s\n", fmt_meta->permissions_str);
    fprintf(file, "Owner: %s\n", fmt_meta->owner_str);
    fprintf(file, "Group: %s\n", fmt_meta->group_str);
    fprintf(file, "Last Modified: %s\n", fmt_meta->time_str);
    fprintf(file, "\n");
}

// Function to verify metadata file
int verify_metadata_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening metadata file for verification");
        return 0;
    }
    
    char line[MAX_LINE];
    int line_count = 0;
    int valid_entries = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_count++;
        if (strstr(line, "File:") || strstr(line, "Type:") || 
            strstr(line, "Size:") || strstr(line, "Inode:")) {
            valid_entries++;
        }
    }
    
    fclose(file);
    
    // Verify that we have complete metadata entries
    return (valid_entries >= 4 && line_count >= 8);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    
    // Open metadata file for writing
    FILE *meta_file = fopen("metadata.txt", "w");
    if (!meta_file) {
        perror("Error opening metadata file");
        return 1;
    }
    
    // Process each file
    for (int i = 1; i < argc; i++) {
        FormattedMetadata fmt_meta;
        format_metadata(argv[i], &fmt_meta);
        write_metadata(meta_file, &fmt_meta);
    }
    
    fclose(meta_file);
    
    // Verify the metadata file
    if (verify_metadata_file("metadata.txt")) {
        printf("Metadata file created and verified successfully.\n");
    } else {
        printf("Warning: Metadata file may be incomplete or corrupted.\n");
    }
    
    return 0;
} 