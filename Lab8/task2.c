#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <time.h>

#define MAX_PROCESSES 10
#define MAX_FILENAME 256
#define MONITOR_INTERVAL 1  // seconds

// Structure to hold process information
typedef struct {
    pid_t pid;
    char state;
    float cpu_usage;
    long memory_usage;
    time_t start_time;
    int is_active;
} ProcessInfo;

// Structure to hold program configuration
typedef struct {
    int process_count;
    int timeout;
    char output_file[MAX_FILENAME];
    ProcessInfo processes[MAX_PROCESSES];
} Config;

// Function to initialize configuration
void init_config(Config *config) {
    memset(config, 0, sizeof(Config));
    config->process_count = 0;
    config->timeout = 60;
    strcpy(config->output_file, "process_log.txt");
}

// Function to get process CPU usage
float get_cpu_usage(pid_t pid) {
    char stat_file[64];
    FILE *fp;
    unsigned long utime, stime, cutime, cstime;
    float cpu_usage = 0.0;
    
    snprintf(stat_file, sizeof(stat_file), "/proc/%d/stat", pid);
    fp = fopen(stat_file, "r");
    if (fp) {
        fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %lu %lu",
               &utime, &stime, &cutime, &cstime);
        fclose(fp);
        cpu_usage = (float)(utime + stime) / sysconf(_SC_CLK_TCK);
    }
    return cpu_usage;
}

// Function to get process memory usage
long get_memory_usage(pid_t pid) {
    char statm_file[64];
    FILE *fp;
    long memory_usage = 0;
    
    snprintf(statm_file, sizeof(statm_file), "/proc/%d/statm", pid);
    fp = fopen(statm_file, "r");
    if (fp) {
        fscanf(fp, "%ld", &memory_usage);
        fclose(fp);
        memory_usage *= sysconf(_SC_PAGESIZE) / 1024;  // Convert to KB
    }
    return memory_usage;
}

// Function to get process state
char get_process_state(pid_t pid) {
    char stat_file[64];
    FILE *fp;
    char state = '?';
    
    snprintf(stat_file, sizeof(stat_file), "/proc/%d/stat", pid);
    fp = fopen(stat_file, "r");
    if (fp) {
        fscanf(fp, "%*d %*s %c", &state);
        fclose(fp);
    }
    return state;
}

// Function to update process information
void update_process_info(ProcessInfo *process) {
    if (process->is_active) {
        process->cpu_usage = get_cpu_usage(process->pid);
        process->memory_usage = get_memory_usage(process->pid);
        process->state = get_process_state(process->pid);
    }
}

// Function to display process hierarchy
void display_process_hierarchy(Config *config) {
    printf("\nProcess Hierarchy:\n");
    printf("Parent (PID: %d)\n", getpid());
    
    for (int i = 0; i < config->process_count; i++) {
        if (config->processes[i].is_active) {
            printf("├── Child %d (PID: %d)\n", i + 1, config->processes[i].pid);
            printf("│   ├── CPU: %.1f%%\n", config->processes[i].cpu_usage);
            printf("│   ├── Memory: %ldMB\n", config->processes[i].memory_usage / 1024);
            printf("│   └── State: %c\n", config->processes[i].state);
        }
    }
}

// Function to display resource usage
void display_resource_usage(Config *config) {
    float total_cpu = 0;
    long total_memory = 0;
    int active_processes = 0;
    
    for (int i = 0; i < config->process_count; i++) {
        if (config->processes[i].is_active) {
            total_cpu += config->processes[i].cpu_usage;
            total_memory += config->processes[i].memory_usage;
            active_processes++;
        }
    }
    
    printf("\nResource Usage:\n");
    printf("Total CPU: %.1f%%\n", total_cpu);
    printf("Total Memory: %ldMB\n", total_memory / 1024);
    printf("Active Processes: %d\n", active_processes);
    printf("Completed Tasks: %d\n", config->process_count - active_processes);
}

// Function to handle child process
void child_process(int id) {
    printf("Child %d (PID: %d) started\n", id, getpid());
    
    // Simulate work
    for (int i = 0; i < 10; i++) {
        sleep(1);
        printf("Child %d: Working... (%d/10)\n", id, i + 1);
    }
    
    printf("Child %d: Completed\n", id);
    exit(0);
}

int main(int argc, char *argv[]) {
    Config config;
    init_config(&config);
    
    // Parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "n:t:o:")) != -1) {
        switch (opt) {
            case 'n':
                config.process_count = atoi(optarg);
                if (config.process_count < 1 || config.process_count > MAX_PROCESSES) {
                    fprintf(stderr, "Error: Process count must be between 1 and %d\n", MAX_PROCESSES);
                    return 1;
                }
                break;
            case 't':
                config.timeout = atoi(optarg);
                if (config.timeout < 1) {
                    fprintf(stderr, "Error: Timeout must be positive\n");
                    return 1;
                }
                break;
            case 'o':
                strncpy(config.output_file, optarg, MAX_FILENAME - 1);
                break;
            default:
                fprintf(stderr, "Usage: %s -n <process_count> -t <timeout> -o <output_file>\n", argv[0]);
                return 1;
        }
    }
    
    if (config.process_count == 0) {
        fprintf(stderr, "Error: Process count (-n) is required\n");
        return 1;
    }
    
    printf("=== Process Management System ===\n");
    printf("Creating %d child processes...\n", config.process_count);
    
    // Create child processes
    for (int i = 0; i < config.process_count; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        } else if (pid == 0) {
            child_process(i + 1);
            return 0;
        } else {
            config.processes[i].pid = pid;
            config.processes[i].is_active = 1;
            config.processes[i].start_time = time(NULL);
        }
    }
    
    // Monitor processes
    time_t start_time = time(NULL);
    while (difftime(time(NULL), start_time) < config.timeout) {
        // Update process information
        for (int i = 0; i < config.process_count; i++) {
            if (config.processes[i].is_active) {
                update_process_info(&config.processes[i]);
                
                // Check if process is still running
                if (kill(config.processes[i].pid, 0) < 0) {
                    config.processes[i].is_active = 0;
                }
            }
        }
        
        // Display process information
        display_process_hierarchy(&config);
        display_resource_usage(&config);
        
        printf("\nMonitoring for %d seconds...\n", config.timeout);
        sleep(MONITOR_INTERVAL);
    }
    
    // Wait for remaining processes
    for (int i = 0; i < config.process_count; i++) {
        if (config.processes[i].is_active) {
            kill(config.processes[i].pid, SIGTERM);
        }
    }
    
    return 0;
} 