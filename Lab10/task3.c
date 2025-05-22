#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

#define MAX_PROCESSES 5
#define MAX_FILENAME 256

// Structure to store process information
typedef struct {
    pid_t pid;
    char name[MAX_FILENAME];
    time_t start_time;
    int is_active;
    int exit_status;
} ProcessInfo;

// Global variables
volatile sig_atomic_t running = 1;
ProcessInfo processes[MAX_PROCESSES];
int process_count = 0;

// Function to get process state
const char* get_process_state(pid_t pid) {
    if (kill(pid, 0) == 0) {
        return "Running";
    }
    return "Terminated";
}

// Function to display process information
void display_processes() {
    printf("\nProcess Information:\n");
    printf("-------------------\n");
    for (int i = 0; i < process_count; i++) {
        if (processes[i].is_active) {
            time_t uptime = time(NULL) - processes[i].start_time;
            printf("Process %d:\n", i + 1);
            printf("  PID: %d\n", processes[i].pid);
            printf("  Name: %s\n", processes[i].name);
            printf("  State: %s\n", get_process_state(processes[i].pid));
            printf("  Uptime: %ld seconds\n", uptime);
        }
    }
}

// Function to start a new process
void start_process(const char* name) {
    if (process_count < MAX_PROCESSES) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return;
        } else if (pid == 0) {
            // Child process
            printf("Child process %d started\n", getpid());
            
            // Set up signal handlers for child
            struct sigaction sa;
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGTERM, &sa, NULL);
            
            // Simulate work
            while (1) {
                sleep(1);
            }
        } else {
            // Parent process
            processes[process_count].pid = pid;
            strncpy(processes[process_count].name, name, MAX_FILENAME - 1);
            processes[process_count].start_time = time(NULL);
            processes[process_count].is_active = 1;
            process_count++;
            
            printf("Started process %d (PID: %d)\n", process_count, pid);
        }
    } else {
        printf("Maximum number of processes reached\n");
    }
}

// Function to stop a process
void stop_process(int process_index) {
    if (process_index >= 0 && process_index < process_count && 
        processes[process_index].is_active) {
        
        pid_t pid = processes[process_index].pid;
        printf("Stopping process %d (PID: %d)...\n", process_index + 1, pid);
        
        // Send SIGTERM first
        kill(pid, SIGTERM);
        sleep(1);
        
        // If still running, force kill with SIGKILL
        if (kill(pid, 0) == 0) {
            printf("Process not responding, sending SIGKILL...\n");
            kill(pid, SIGKILL);
        }
        
        processes[process_index].is_active = 0;
        printf("Process stopped\n");
    } else {
        printf("Invalid process index or process not active\n");
    }
}

// Signal handler
void signal_handler(int signum) {
    switch (signum) {
        case SIGINT:  // Ctrl+C
            printf("\nReceived SIGINT. Initiating graceful shutdown...\n");
            running = 0;
            break;
            
        case SIGUSR1:  // Display processes
            display_processes();
            break;
            
        case SIGUSR2:  // Start new process
            start_process("worker");
            break;
    }
}

int main() {
    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    printf("Process Control System (PID: %d)\n", getpid());
    printf("Controls:\n");
    printf("- Ctrl+C: Initiate graceful shutdown\n");
    printf("- SIGUSR1: Display process information\n");
    printf("- SIGUSR2: Start a new process\n");
    printf("\nSystem started...\n");
    
    // Main loop
    while (running) {
        // Check for terminated processes
        for (int i = 0; i < process_count; i++) {
            if (processes[i].is_active && kill(processes[i].pid, 0) != 0) {
                printf("Process %d (PID: %d) has terminated\n", 
                       i + 1, processes[i].pid);
                processes[i].is_active = 0;
            }
        }
        sleep(1);
    }
    
    // Graceful shutdown
    printf("\nPerforming graceful shutdown...\n");
    for (int i = 0; i < process_count; i++) {
        if (processes[i].is_active) {
            stop_process(i);
        }
    }
    
    printf("System shutdown complete\n");
    return 0;
} 