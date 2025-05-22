#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/stat.h>
#include <json-c/json.h>

#define VERSION "2.1"
#define MAX_FILES 10
#define MAX_PATH 256

// Structure to hold program configuration
typedef struct {
    char config_file[MAX_PATH];
    char output_file[MAX_PATH];
    int verbose;
    int thread_count;
    char input_files[MAX_FILES][MAX_PATH];
    int input_count;
} Config;

// Function to initialize configuration
void init_config(Config *config) {
    memset(config, 0, sizeof(Config));
    strcpy(config->config_file, "config.json");
    strcpy(config->output_file, "output.txt");
    config->verbose = 0;
    config->thread_count = 1;
    config->input_count = 0;
}

// Function to load configuration from JSON file
int load_config(Config *config) {
    struct json_object *json;
    json = json_object_from_file(config->config_file);
    
    if (!json) {
        fprintf(stderr, "Error: Could not load config file: %s\n", config->config_file);
        return 0;
    }
    
    struct json_object *value;
    if (json_object_object_get_ex(json, "thread_count", &value)) {
        config->thread_count = json_object_get_int(value);
    }
    if (json_object_object_get_ex(json, "verbose", &value)) {
        config->verbose = json_object_get_boolean(value);
    }
    
    json_object_put(json);
    return 1;
}

// Function to display help information
void display_help() {
    printf("Usage: task1 [OPTIONS] [FILES]\n");
    printf("Options:\n");
    printf("  -c, --config FILE    Specify configuration file (default: config.json)\n");
    printf("  -o, --output FILE    Specify output file (default: output.txt)\n");
    printf("  -t, --threads N      Number of threads to use (default: 1)\n");
    printf("  -v, --verbose        Enable verbose output\n");
    printf("  -h, --help           Display this help message\n");
    printf("  -V, --version        Display version information\n");
}

// Function to display version information
void display_version() {
    printf("Command Line Parser v%s\n", VERSION);
    printf("Copyright (C) 2024 System Programming Lab\n");
}

// Function to validate file existence
int validate_file(const char *filename) {
    struct stat st;
    return (stat(filename, &st) == 0);
}

// Function to process input files
void process_files(Config *config) {
    printf("\nProcessing input files:\n");
    for (int i = 0; i < config->input_count; i++) {
        struct stat st;
        if (stat(config->input_files[i], &st) == 0) {
            printf("  - %s (%.1fMB)\n", 
                   config->input_files[i], 
                   (float)st.st_size / (1024 * 1024));
        } else {
            fprintf(stderr, "Warning: File not found: %s\n", config->input_files[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    Config config;
    init_config(&config);
    
    static struct option long_options[] = {
        {"config",   required_argument, 0, 'c'},
        {"output",   required_argument, 0, 'o'},
        {"threads",  required_argument, 0, 't'},
        {"verbose",  no_argument,       0, 'v'},
        {"help",     no_argument,       0, 'h'},
        {"version",  no_argument,       0, 'V'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "c:o:t:vhV", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                strncpy(config.config_file, optarg, MAX_PATH - 1);
                break;
            case 'o':
                strncpy(config.output_file, optarg, MAX_PATH - 1);
                break;
            case 't':
                config.thread_count = atoi(optarg);
                if (config.thread_count < 1) {
                    fprintf(stderr, "Error: Thread count must be positive\n");
                    return 1;
                }
                break;
            case 'v':
                config.verbose = 1;
                break;
            case 'h':
                display_help();
                return 0;
            case 'V':
                display_version();
                return 0;
            default:
                fprintf(stderr, "Try '%s --help' for more information\n", argv[0]);
                return 1;
        }
    }
    
    // Process remaining arguments as input files
    while (optind < argc && config.input_count < MAX_FILES) {
        strncpy(config.input_files[config.input_count], 
                argv[optind], 
                MAX_PATH - 1);
        config.input_count++;
        optind++;
    }
    
    // Load configuration from file
    if (!load_config(&config)) {
        return 1;
    }
    
    // Display configuration
    printf("=== Command Line Parser v%s ===\n", VERSION);
    printf("Options processed:\n");
    printf("  --config: %s\n", config.config_file);
    printf("  --verbose: %s\n", config.verbose ? "enabled" : "disabled");
    printf("  --output: %s\n", config.output_file);
    printf("  --threads: %d\n", config.thread_count);
    
    if (config.input_count > 0) {
        printf("\nArguments received:\n");
        for (int i = 0; i < config.input_count; i++) {
            printf("  %s\n", config.input_files[i]);
        }
    }
    
    printf("\nConfiguration loaded from: %s\n", config.config_file);
    printf("Verbose mode: %s\n", config.verbose ? "ON" : "OFF");
    printf("Output file: %s\n", config.output_file);
    printf("Thread count: %d\n", config.thread_count);
    
    // Process input files
    process_files(&config);
    
    printf("\nStatus: Ready to process\n");
    return 0;
} 