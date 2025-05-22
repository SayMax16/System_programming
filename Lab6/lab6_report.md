# Lab 6: Advanced File Handling in Linux

This lab focuses on advanced file handling in Linux using system calls and stdio functions. The implementation includes five unique tasks that demonstrate different aspects of file operations.

## Task 1: File Offset Manipulation (`task1.c`)

This task demonstrates the use of `lseek` to manipulate file offsets and read data from specific positions.

### Features:
- File existence check before opening
- Multiple offset positions for reading
- Content verification against expected values
- Detailed error reporting

### Implementation:
```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_OFFSETS 5


typedef struct {
    off_t offset;
    size_t length;
    char description[50];
} OffsetInfo;


int verify_content(int fd, off_t offset, size_t length, const char *expected) {
    char buffer[BUFFER_SIZE];
    
    
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking in file");
        return 0;
    }
    
    
    ssize_t bytes_read = read(fd, buffer, length);
    if (bytes_read == -1) {
        perror("Error reading file");
        return 0;
    }
    
    buffer[bytes_read] = '\0';
    
    return (strcmp(buffer, expected) == 0);
}


void print_content_at_offset(int fd, off_t offset, size_t length) {
    char buffer[BUFFER_SIZE];
    
    
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
    
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    
    
    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("Error getting file information");
        close(fd);
        return 1;
    }
    
    printf("File: %s\n", filename);
    printf("Size: %ld bytes\n", (long)file_stat.st_size);
    
    
    OffsetInfo offsets[] = {
        {0, 10, "Start of file"},
        {5, 15, "Middle section"},
        {file_stat.st_size - 10, 10, "End of file"},
        {20, 20, "Custom section"},
        {40, 15, "Final section"}
    };
    
    
    for (int i = 0; i < MAX_OFFSETS; i++) {
        printf("\nChecking %s:\n", offsets[i].description);
        print_content_at_offset(fd, offsets[i].offset, offsets[i].length);
        
        
        if (argc > 2) {
            const char *expected = argv[2];
            if (verify_content(fd, offsets[i].offset, offsets[i].length, expected)) {
                printf("Content verification successful!\n");
            } else {
                printf("Content verification failed!\n");
            }
        }
    }
    
    
    if (close(fd) == -1) {
        perror("Error closing file");
        return 1;
    }
    
    return 0;
}

### Actual Output
```
File: test.txt
Size: 30 bytes

Checking Start of file:
Content at offset 0: This is a 

Checking Middle section:
Content at offset 5: is a test file 

Checking End of file:
Content at offset 20: for task1


Checking Custom section:
Content at offset 20: for task1


Checking Final section:
Content at offset 40: 
```

## Task 2: File Metadata Analysis (`task2.c`)

This task implements detailed file metadata analysis using `stat` and `lstat` system calls.

### Features:
- Comprehensive metadata collection
- Symbolic link handling
- Metadata comparison between files
- Formatted output with permissions and timestamps

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define MAX_PATH 256

typedef struct {
    char path[MAX_PATH];
    struct stat stat_info;
    char type[20];
    char permissions[10];
    char last_modified[50];
    char last_accessed[50];
    char created[50];
} FileMetadata;


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
    
    
    get_file_type(metadata->stat_info.st_mode, metadata->type);
    
    
    get_permissions(metadata->stat_info.st_mode, metadata->permissions);
    
    
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
    
    
    if (!get_file_metadata(argv[1], &meta1, 1)) {
        perror("Error getting file metadata");
        return 1;
    }
    
    printf("=== File Metadata (using stat) ===\n");
    print_metadata(&meta1);
    
    
    if (!get_file_metadata(argv[1], &meta2, 0)) {
        perror("Error getting file metadata");
        return 1;
    }
    
    printf("\n=== File Metadata (using lstat) ===\n");
    print_metadata(&meta2);
    
    
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

### Actual Output
```
=== File Metadata (using stat) ===

File: file1.txt
Type: Regular File
Size: 22 bytes
Permissions: rw-r--r--
Inode: 10207604
Links: 1
Owner: 501
Group: 20
Last Modified: 2025-05-22 14:29:49
Last Accessed: 2025-05-22 14:29:49
Created: 2025-05-22 14:29:49

=== File Metadata (using lstat) ===

File: file1.txt
Type: Regular File
Size: 22 bytes
Permissions: rw-r--r--
Inode: 10207604
Links: 1
Owner: 501
Group: 20
Last Modified: 2025-05-22 14:29:49
Last Accessed: 2025-05-22 14:29:49
Created: 2025-05-22 14:29:49

=== Second File Metadata ===

File: file2.txt
Type: Regular File
Size: 22 bytes
Permissions: rw-r--r--
Inode: 10207605
Links: 1
Owner: 501
Group: 20
Last Modified: 2025-05-22 14:29:49
Last Accessed: 2025-05-22 14:29:49
Created: 2025-05-22 14:29:49

Comparing file1.txt and file2.txt:
Size: Same
Inode: Different
Type: Same
Permissions: Same
```

## Task 3: File Descriptor Management (`task3.c`)

This task demonstrates file descriptor duplication and redirection using `dup` and `dup2`.

### Features:
- Safe file descriptor operations
- Multiple output file handling
- Error handling for all operations
- Progress tracking

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024
#define MAX_FILES 3


typedef struct {
    int fd;
    char path[256];
    int flags;
    mode_t mode;
} FileInfo;


void safe_close(int fd, const char *filename) {
    if (close(fd) == -1) {
        fprintf(stderr, "Error closing %s: %s\n", filename, strerror(errno));
    }
}


int safe_dup(int oldfd, const char *filename) {
    int newfd = dup(oldfd);
    if (newfd == -1) {
        fprintf(stderr, "Error duplicating %s: %s\n", filename, strerror(errno));
    }
    return newfd;
}


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
    
    
    files[0].fd = open(files[0].path, files[0].flags);
    if (files[0].fd == -1) {
        perror("Error opening input file");
        return 1;
    }
    
    
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
    
    
    read_and_display(files[0].fd, files[0].path);
    
    
    int dup_fd = safe_dup(files[0].fd, files[0].path);
    if (dup_fd != -1) {
        printf("\nUsing duplicated file descriptor:\n");
        read_and_display(dup_fd, files[0].path);
        safe_close(dup_fd, "duplicated descriptor");
    }
    
    
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
    
    
    write_to_file(files[2].fd, files[2].path, "This message goes to output2.txt\n");
    
    
    for (int i = 0; i < MAX_FILES; i++) {
        safe_close(files[i].fd, files[i].path);
    }
    
    printf("\nFile operations completed. Check output1.txt and output2.txt for results.\n");
    
    return 0;
}

### Actual Output
```
Reading from input.txt:
----------------------------------------
Line 1
Line 2
Line 3
----------------------------------------

Using duplicated file descriptor:

Reading from input.txt:
----------------------------------------
----------------------------------------

File operations completed. Check output1.txt and output2.txt for results.
```

## Task 4: Formatted Metadata Writing (`task4.c`)

This task implements formatted file metadata writing with verification.

### Features:
- Custom metadata formatting
- Multiple file processing
- File integrity verification
- Detailed error reporting

### Implementation:
```c
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


void format_time(time_t time_val, char *time_str) {
    struct tm *tm_info = localtime(&time_val);
    char timezone[10];
    strftime(timezone, sizeof(timezone), "%z", tm_info);
    strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
    strcat(time_str, " ");
    strcat(time_str, timezone);
}


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
    
    
    return (valid_entries >= 4 && line_count >= 8);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2] ...\n", argv[0]);
        return 1;
    }
    
    
    FILE *meta_file = fopen("metadata.txt", "w");
    if (!meta_file) {
        perror("Error opening metadata file");
        return 1;
    }
    
    
    for (int i = 1; i < argc; i++) {
        FormattedMetadata fmt_meta;
        format_metadata(argv[i], &fmt_meta);
        write_metadata(meta_file, &fmt_meta);
    }
    
    fclose(meta_file);
    
    
    if (verify_metadata_file("metadata.txt")) {
        printf("Metadata file created and verified successfully.\n");
    } else {
        printf("Warning: Metadata file may be incomplete or corrupted.\n");
    }
    
    return 0;
}

### Actual Output
```
Metadata file created and verified successfully.
```

## Task 5: Data Parsing and Validation (`task5.c`)

This task implements data parsing and validation from input files.

### Features:
- CSV format parsing
- Comprehensive data validation
- Custom validation rules
- Detailed error reporting

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE 1024
#define MAX_FIELDS 10
#define MAX_FIELD_LENGTH 100


typedef struct {
    char name[MAX_FIELD_LENGTH];
    int id;
    double value;
    char category[MAX_FIELD_LENGTH];
    char timestamp[MAX_FIELD_LENGTH];
} ParsedData;


typedef struct {
    int min_id;
    int max_id;
    double min_value;
    double max_value;
    char valid_categories[][MAX_FIELD_LENGTH];
    int num_categories;
} ValidationRules;


void init_validation_rules(ValidationRules *rules) {
    rules->min_id = 1;
    rules->max_id = 999999;
    rules->min_value = 0.0;
    rules->max_value = 1000000.0;
    
    const char *categories[] = {
        "A", "B", "C", "D", "E"
    };
    rules->num_categories = sizeof(categories) / sizeof(categories[0]);
    
    for (int i = 0; i < rules->num_categories; i++) {
        strncpy(rules->valid_categories[i], categories[i], MAX_FIELD_LENGTH - 1);
        rules->valid_categories[i][MAX_FIELD_LENGTH - 1] = '\0';
    }
}


int validate_data(const ParsedData *data, const ValidationRules *rules) {
    
    if (data->id < rules->min_id || data->id > rules->max_id) {
        fprintf(stderr, "Error: Invalid ID %d (must be between %d and %d)\n",
                data->id, rules->min_id, rules->max_id);
        return 0;
    }
    
    
    if (data->value < rules->min_value || data->value > rules->max_value) {
        fprintf(stderr, "Error: Invalid value %.2f (must be between %.2f and %.2f)\n",
                data->value, rules->min_value, rules->max_value);
        return 0;
    }
    
    
    int valid_category = 0;
    for (int i = 0; i < rules->num_categories; i++) {
        if (strcmp(data->category, rules->valid_categories[i]) == 0) {
            valid_category = 1;
            break;
        }
    }
    if (!valid_category) {
        fprintf(stderr, "Error: Invalid category '%s'\n", data->category);
        return 0;
    }
    
    
    if (strlen(data->timestamp) != 19) {
        fprintf(stderr, "Error: Invalid timestamp format\n");
        return 0;
    }
    
    
    for (int i = 0; i < 19; i++) {
        if (i == 4 || i == 7) {
            if (data->timestamp[i] != '-') {
                fprintf(stderr, "Error: Invalid timestamp format (expected '-')\n");
                return 0;
            }
        } else if (i == 10) {
            if (data->timestamp[i] != ' ') {
                fprintf(stderr, "Error: Invalid timestamp format (expected space)\n");
                return 0;
            }
        } else if (i == 13 || i == 16) {
            if (data->timestamp[i] != ':') {
                fprintf(stderr, "Error: Invalid timestamp format (expected ':')\n");
                return 0;
            }
        } else {
            if (!isdigit(data->timestamp[i])) {
                fprintf(stderr, "Error: Invalid timestamp format (expected digit)\n");
                return 0;
            }
        }
    }
    
    return 1;
}


int parse_line(const char *line, ParsedData *data) {
    char name[MAX_FIELD_LENGTH];
    int id;
    double value;
    char category[MAX_FIELD_LENGTH];
    char timestamp[MAX_FIELD_LENGTH];
    
    // Parse line using sscanf
    int result = sscanf(line, "%[^,],%d,%lf,%[^,],%s",
                       name, &id, &value, category, timestamp);
    
    if (result != 5) {
        fprintf(stderr, "Error: Invalid line format\n");
        return 0;
    }
    
    
    strncpy(data->name, name, MAX_FIELD_LENGTH - 1);
    data->name[MAX_FIELD_LENGTH - 1] = '\0';
    data->id = id;
    data->value = value;
    strncpy(data->category, category, MAX_FIELD_LENGTH - 1);
    data->category[MAX_FIELD_LENGTH - 1] = '\0';
    strncpy(data->timestamp, timestamp, MAX_FIELD_LENGTH - 1);
    data->timestamp[MAX_FIELD_LENGTH - 1] = '\0';
    
    return 1;
}


void print_parsed_data(const ParsedData *data) {
    printf("\nParsed Data:\n");
    printf("Name: %s\n", data->name);
    printf("ID: %d\n", data->id);
    printf("Value: %.2f\n", data->value);
    printf("Category: %s\n", data->category);
    printf("Timestamp: %s\n", data->timestamp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
    
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening input file");
        return 1;
    }
    
    
    ValidationRules rules;
    init_validation_rules(&rules);
    
    
    char line[MAX_LINE];
    int line_number = 0;
    int valid_lines = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        
        
        line[strcspn(line, "\n")] = '\0';
        
        if (strlen(line) == 0) continue;
        
        ParsedData data;
        if (!parse_line(line, &data)) {
            fprintf(stderr, "Error parsing line %d\n", line_number);
            continue;
        }
        
        if (!validate_data(&data, &rules)) {
            fprintf(stderr, "Error validating line %d\n", line_number);
            continue;
        }
        
        print_parsed_data(&data);
        valid_lines++;
    }
    
    fclose(file);
    
    printf("\nProcessing complete:\n");
    printf("Total lines processed: %d\n", line_number);
    printf("Valid lines: %d\n", valid_lines);
    printf("Invalid lines: %d\n", line_number - valid_lines);
    
    return 0;
}

### Actual Output
```
Error: Invalid timestamp format
Error validating line 1
Error: Invalid timestamp format
Error validating line 2
Error: Invalid ID 0 (must be between 1 and 999999)
Error validating line 3

Processing complete:
Total lines processed: 3
Valid lines: 0
Invalid lines: 3
```

## Building and Running

To build all tasks:
```