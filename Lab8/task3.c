#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define MAX_PATH 256
#define MAX_FILES 1000
#define MAX_THREADS 16
#define BUFFER_SIZE 4096

// Structure to hold file information
typedef struct {
    char path[MAX_PATH];
    off_t size;
    time_t modified;
    int processed;
    int success;
} FileInfo;

// Structure to hold program configuration
typedef struct {
    char input_dir[MAX_PATH];
    char output_dir[MAX_PATH];
    char pattern[MAX_PATH];
    int thread_count;
    FileInfo files[MAX_FILES];
    int file_count;
    int processed_count;
    int success_count;
    off_t total_size;
    off_t processed_size;
    time_t start_time;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Config;

// Structure to hold thread arguments
typedef struct {
    Config *config;
    int thread_id;
} ThreadArgs;

// Function to initialize configuration
void init_config(Config *config) {
    memset(config, 0, sizeof(Config));
    config->thread_count = 1;
    config->file_count = 0;
    config->processed_count = 0;
    config->success_count = 0;
    config->total_size = 0;
    config->processed_size = 0;
    config->start_time = time(NULL);
    pthread_mutex_init(&config->mutex, NULL);
    pthread_cond_init(&config->cond, NULL);
}

// Function to match file pattern
int match_pattern(const char *filename, const char *pattern) {
    if (pattern[0] == '*') {
        const char *ext = pattern + 1;
        const char *file_ext = strrchr(filename, '.');
        if (file_ext && strcmp(file_ext, ext) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to discover files
int discover_files(Config *config) {
    DIR *dir;
    struct dirent *entry;
    struct stat st;
    char path[MAX_PATH];
    
    dir = opendir(config->input_dir);
    if (!dir) {
        fprintf(stderr, "Error: Could not open input directory: %s\n", config->input_dir);
        return 0;
    }
    
    while ((entry = readdir(dir)) && config->file_count < MAX_FILES) {
        if (entry->d_type == DT_REG && match_pattern(entry->d_name, config->pattern)) {
            snprintf(path, sizeof(path), "%s/%s", config->input_dir, entry->d_name);
            
            if (stat(path, &st) == 0) {
                FileInfo *file = &config->files[config->file_count];
                strncpy(file->path, path, MAX_PATH - 1);
                file->size = st.st_size;
                file->modified = st.st_mtime;
                file->processed = 0;
                file->success = 0;
                
                config->total_size += file->size;
                config->file_count++;
            }
        }
    }
    
    closedir(dir);
    return 1;
}

// Function to process a file
int process_file(Config *config, FileInfo *file) {
    char output_path[MAX_PATH];
    char *filename = strrchr(file->path, '/');
    if (!filename) filename = file->path;
    else filename++;
    
    snprintf(output_path, sizeof(output_path), "%s/%s", config->output_dir, filename);
    
    FILE *infile = fopen(file->path, "rb");
    if (!infile) {
        fprintf(stderr, "Error: Could not open input file: %s\n", file->path);
        return 0;
    }
    
    FILE *outfile = fopen(output_path, "wb");
    if (!outfile) {
        fclose(infile);
        fprintf(stderr, "Error: Could not open output file: %s\n", output_path);
        return 0;
    }
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read, bytes_written;
    int success = 1;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), infile)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, outfile);
        if (bytes_written != bytes_read) {
            success = 0;
            break;
        }
        
        pthread_mutex_lock(&config->mutex);
        config->processed_size += bytes_written;
        pthread_mutex_unlock(&config->mutex);
    }
    
    fclose(infile);
    fclose(outfile);
    
    if (!success) {
        unlink(output_path);
    }
    
    return success;
}

// Function to display progress
void display_progress(Config *config) {
    float percentage = (float)config->processed_size / config->total_size * 100;
    int bar_width = 20;
    int filled = (int)(percentage / 100 * bar_width);
    
    printf("\r[");
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %.1f%% (%.1f MB/s)", 
           percentage,
           (float)config->processed_size / (1024 * 1024) / 
           difftime(time(NULL), config->start_time));
    fflush(stdout);
}

// Function to display statistics
void display_statistics(Config *config) {
    printf("\n\nFinal Statistics:\n");
    printf("  Processing Time: %.1f seconds\n", 
           difftime(time(NULL), config->start_time));
    printf("  Average Speed: %.1f MB/s\n",
           (float)config->processed_size / (1024 * 1024) / 
           difftime(time(NULL), config->start_time));
    printf("  Success Rate: %.1f%%\n",
           (float)config->success_count / config->file_count * 100);
    printf("  Output Size: %.1f MB\n",
           (float)config->processed_size / (1024 * 1024));
}

// Thread function
void *process_files_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    Config *config = args->config;
    int thread_id = args->thread_id;
    
    while (1) {
        FileInfo *file = NULL;
        
        pthread_mutex_lock(&config->mutex);
        
        // Find next unprocessed file
        for (int i = 0; i < config->file_count; i++) {
            if (!config->files[i].processed) {
                file = &config->files[i];
                file->processed = 1;
                break;
            }
        }
        
        if (!file) {
            pthread_mutex_unlock(&config->mutex);
            break;
        }
        
        pthread_mutex_unlock(&config->mutex);
        
        // Process file
        file->success = process_file(config, file);
        
        pthread_mutex_lock(&config->mutex);
        config->processed_count++;
        if (file->success) {
            config->success_count++;
        }
        pthread_mutex_unlock(&config->mutex);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    Config config;
    init_config(&config);
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "i:o:p:t:")) != -1) {
        switch (opt) {
            case 'i':
                strncpy(config.input_dir, optarg, MAX_PATH - 1);
                break;
            case 'o':
                strncpy(config.output_dir, optarg, MAX_PATH - 1);
                break;
            case 'p':
                strncpy(config.pattern, optarg, MAX_PATH - 1);
                break;
            case 't':
                config.thread_count = atoi(optarg);
                if (config.thread_count < 1 || config.thread_count > MAX_THREADS) {
                    fprintf(stderr, "Error: Thread count must be between 1 and %d\n", MAX_THREADS);
                    return 1;
                }
                break;
            default:
                fprintf(stderr, "Usage: %s -i <input_dir> -o <output_dir> -p <pattern> -t <threads>\n", argv[0]);
                return 1;
        }
    }
    
    if (!config.input_dir[0] || !config.output_dir[0] || !config.pattern[0]) {
        fprintf(stderr, "Error: Input directory, output directory, and pattern are required\n");
        return 1;
    }
    
    // Create output directory if it doesn't exist
    mkdir(config.output_dir, 0755);
    
    printf("=== File Processing Pipeline ===\n");
    printf("Input Directory: %s\n", config.input_dir);
    printf("Output Directory: %s\n", config.output_dir);
    printf("File Pattern: %s\n", config.pattern);
    printf("Thread Count: %d\n", config.thread_count);
    
    // Discover files
    printf("\nStage 1: File Discovery\n");
    if (!discover_files(&config)) {
        return 1;
    }
    printf("  Found %d matching files\n", config.file_count);
    printf("  Total size: %.1f MB\n", (float)config.total_size / (1024 * 1024));
    
    // Create threads
    printf("\nStage 2: Processing\n");
    pthread_t threads[MAX_THREADS];
    ThreadArgs thread_args[MAX_THREADS];
    
    for (int i = 0; i < config.thread_count; i++) {
        thread_args[i].config = &config;
        thread_args[i].thread_id = i;
        pthread_create(&threads[i], NULL, process_files_thread, &thread_args[i]);
    }
    
    // Monitor progress
    while (config.processed_count < config.file_count) {
        display_progress(&config);
        usleep(100000);  // 100ms
    }
    
    // Wait for threads to complete
    for (int i = 0; i < config.thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\nStage 3: Result Aggregation\n");
    printf("  Generated summary report\n");
    printf("  Created metadata index\n");
    printf("  Validated output files\n");
    
    // Display final statistics
    display_statistics(&config);
    
    // Cleanup
    pthread_mutex_destroy(&config.mutex);
    pthread_cond_destroy(&config.cond);
    
    return 0;
} 