#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>

#define MAX_PROCESSES 5
#define WORK_ITERATIONS 1000000

// Structure to hold process information
typedef struct {
    pid_t pid;
    int priority;
    int cpu_usage;
    int memory_usage;
    time_t start_time;
    int is_active;
} ProcessInfo;

// Function to perform CPU-intensive work
void do_work() {
    volatile double result = 0.0;
    for (int i = 0; i < WORK_ITERATIONS; i++) {
        result += i * i;
    }
}

// Function to get process CPU usage
int get_cpu_usage(pid_t pid) {
    char stat_file[64];
    FILE *fp;
    unsigned long utime, stime;
    int cpu_usage = 0;
    
    snprintf(stat_file, sizeof(stat_file), "/proc/%d/stat", pid);
    fp = fopen(stat_file, "r");
    if (fp) {
        fscanf(fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
               &utime, &stime);
        fclose(fp);
        cpu_usage = (int)((utime + stime) / sysconf(_SC_CLK_TCK));
    }
    return cpu_usage;
}

// Function to get process memory usage
int get_memory_usage(pid_t pid) {
    char statm_file[64];
    FILE *fp;
    int memory_usage = 0;
    
    snprintf(statm_file, sizeof(statm_file), "/proc/%d/statm", pid);
    fp = fopen(statm_file, "r");
    if (fp) {
        fscanf(fp, "%d", &memory_usage);
        fclose(fp);
        memory_usage *= sysconf(_SC_PAGESIZE) / 1024;  // Convert to KB
    }
    return memory_usage;
}

// Function to set process priority
void set_process_priority(pid_t pid, int priority) {
    if (setpriority(PRIO_PROCESS, pid, priority) != 0) {
        perror("setpriority failed");
    }
}

// Function to display process information
void display_process_info(ProcessInfo* processes, int count) {
    printf("\nProcess Information:\n");
    printf("PID\tPriority\tCPU Usage\tMemory Usage\tActive Time\n");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        if (processes[i].is_active) {
            time_t active_time = time(NULL) - processes[i].start_time;
            printf("%d\t%d\t\t%d%%\t\t%d KB\t\t%ld s\n",
                   processes[i].pid,
                   processes[i].priority,
                   processes[i].cpu_usage,
                   processes[i].memory_usage,
                   active_time);
        }
    }
}

// Function to update process information
void update_process_info(ProcessInfo* process) {
    if (process->is_active) {
        process->cpu_usage = get_cpu_usage(process->pid);
        process->memory_usage = get_memory_usage(process->pid);
    }
}

// Child process function
void child_process(int id, int priority) {
    printf("Child %d (PID: %d) started with priority %d\n", id, getpid(), priority);
    
    // Set process priority
    set_process_priority(getpid(), priority);
    
    // Perform work
    for (int i = 0; i < 5; i++) {
        printf("Child %d: Working... (%d/5)\n", id, i + 1);
        do_work();
        sleep(1);
    }
    
    printf("Child %d: Completed\n", id);
    exit(0);
}

int main() {
    ProcessInfo processes[MAX_PROCESSES];
    int priorities[] = {10, 0, -10, -5, 5};  // Different priorities for each process
    
    printf("=== Process Priority Management System ===\n");
    
    // Create child processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        } else if (pid == 0) {
            child_process(i + 1, priorities[i]);
            return 0;
        } else {
            processes[i].pid = pid;
            processes[i].priority = priorities[i];
            processes[i].cpu_usage = 0;
            processes[i].memory_usage = 0;
            processes[i].start_time = time(NULL);
            processes[i].is_active = 1;
        }
    }
    
    // Monitor processes
    int active_processes = MAX_PROCESSES;
    while (active_processes > 0) {
        // Update process information
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].is_active) {
                update_process_info(&processes[i]);
                
                // Check if process is still running
                if (kill(processes[i].pid, 0) < 0) {
                    processes[i].is_active = 0;
                    active_processes--;
                }
            }
        }
        
        // Display process information
        display_process_info(processes, MAX_PROCESSES);
        
        // Wait before next update
        sleep(2);
    }
    
    // Wait for all processes to complete
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int status;
        waitpid(processes[i].pid, &status, 0);
    }
    
    printf("\nAll processes have completed\n");
    return 0;
} 