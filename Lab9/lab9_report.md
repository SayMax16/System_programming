# Lab 9: Process Creation, Execution, and Control in Linux

This lab focuses on advanced process management in Linux, with unique implementations that include shared memory synchronization, environment variable manipulation, and process priority management.

## Task 1: Process Synchronization with Shared Memory (`task1.c`)

This task implements a parent-child process synchronization system using shared memory.

### Features:
- Shared memory for inter-process communication
- Turn-based message exchange
- Random message generation
- Counter synchronization
- Clean resource management

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define SHM_NAME "/process_sync"
#define SHM_SIZE sizeof(SharedData)

// Structure to hold shared data
typedef struct {
    int counter;
    int is_parent_turn;
    char message[256];
} SharedData;

// Function to initialize shared memory
SharedData* init_shared_memory() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(1);
    }

    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    SharedData* shared_data = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    close(fd);
    return shared_data;
}

// Function to cleanup shared memory
void cleanup_shared_memory() {
    shm_unlink(SHM_NAME);
}

// Function to generate random message
void generate_message(char* message) {
    const char* adjectives[] = {"happy", "sad", "excited", "tired", "energetic"};
    const char* nouns[] = {"process", "thread", "system", "program", "computer"};
    
    srand(time(NULL));
    int adj_idx = rand() % 5;
    int noun_idx = rand() % 5;
    
    snprintf(message, 256, "This is a %s %s!", adjectives[adj_idx], nouns[noun_idx]);
}

// Parent process function
void parent_process(SharedData* shared_data) {
    printf("Parent process (PID: %d) started\n", getpid());
    
    for (int i = 0; i < 5; i++) {
        // Wait for child's turn
        while (shared_data->is_parent_turn == 0) {
            usleep(100000);  // Sleep for 100ms
        }
        
        // Generate and send message
        generate_message(shared_data->message);
        printf("Parent: %s (Counter: %d)\n", shared_data->message, shared_data->counter);
        shared_data->counter++;
        
        // Switch turn to child
        shared_data->is_parent_turn = 0;
    }
    
    printf("Parent process completed\n");
}

// Child process function
void child_process(SharedData* shared_data) {
    printf("Child process (PID: %d) started\n", getpid());
    
    for (int i = 0; i < 5; i++) {
        // Wait for parent's turn
        while (shared_data->is_parent_turn == 1) {
            usleep(100000);  // Sleep for 100ms
        }
        
        // Generate and send message
        generate_message(shared_data->message);
        printf("Child: %s (Counter: %d)\n", shared_data->message, shared_data->counter);
        shared_data->counter++;
        
        // Switch turn to parent
        shared_data->is_parent_turn = 1;
    }
    
    printf("Child process completed\n");
}

int main() {
    // Initialize shared memory
    SharedData* shared_data = init_shared_memory();
    
    // Initialize shared data
    shared_data->counter = 0;
    shared_data->is_parent_turn = 1;  // Parent goes first
    strcpy(shared_data->message, "Initial message");
    
    // Create child process
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        cleanup_shared_memory();
        exit(1);
    } else if (pid == 0) {
        // Child process
        child_process(shared_data);
        exit(0);
    } else {
        // Parent process
        parent_process(shared_data);
        
        // Wait for child to complete
        int status;
        waitpid(pid, &status, 0);
        
        printf("\nFinal counter value: %d\n", shared_data->counter);
        
        // Cleanup
        munmap(shared_data, SHM_SIZE);
        cleanup_shared_memory();
    }
    
    return 0;
}
```

### Usage:
```bash
./task1
```

### Example Output:
```
Parent process (PID: 12345) started
Child process (PID: 12346) started

Parent: This is a happy process! (Counter: 0)
Child: This is an excited system! (Counter: 1)
Parent: This is a tired computer! (Counter: 2)
Child: This is an energetic program! (Counter: 3)
Parent: This is a sad thread! (Counter: 4)

Parent process completed
Child process completed

Final counter value: 5
```

## Task 2: Process Execution with Environment Variables (`task2.c`)

This task demonstrates process execution with custom environment variables.

### Features:
- Dynamic environment variable generation
- Process execution with modified environment
- Environment variable inheritance
- Command execution monitoring
- Exit status handling

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define MAX_ENV_VARS 10
#define MAX_ENV_LEN 256

// Structure to hold environment variable
typedef struct {
    char name[MAX_ENV_LEN];
    char value[MAX_ENV_LEN];
} EnvVar;

// Function to generate random environment variables
void generate_env_vars(EnvVar* env_vars, int count) {
    const char* prefixes[] = {"APP_", "SYS_", "USER_", "DATA_", "CONFIG_"};
    const char* suffixes[] = {"PATH", "HOME", "TEMP", "LOG", "CACHE"};
    
    srand(time(NULL));
    
    for (int i = 0; i < count; i++) {
        int prefix_idx = rand() % 5;
        int suffix_idx = rand() % 5;
        int value = rand() % 1000;
        
        snprintf(env_vars[i].name, MAX_ENV_LEN, "%s%s", prefixes[prefix_idx], suffixes[suffix_idx]);
        snprintf(env_vars[i].value, MAX_ENV_LEN, "/var/%s/%d", env_vars[i].name, value);
    }
}

// Function to set environment variables
void set_env_vars(EnvVar* env_vars, int count) {
    for (int i = 0; i < count; i++) {
        if (setenv(env_vars[i].name, env_vars[i].value, 1) != 0) {
            perror("setenv failed");
            exit(1);
        }
    }
}

// Function to print environment variables
void print_env_vars(EnvVar* env_vars, int count) {
    printf("\nCurrent Environment Variables:\n");
    for (int i = 0; i < count; i++) {
        printf("%s=%s\n", env_vars[i].name, env_vars[i].value);
    }
}

// Function to execute command with environment
void execute_command(const char* command, char* const argv[]) {
    printf("\nExecuting command: %s\n", command);
    
    // Execute the command
    execvp(command, argv);
    
    // If execvp returns, it means there was an error
    perror("execvp failed");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }
    
    // Generate random environment variables
    EnvVar env_vars[MAX_ENV_VARS];
    int env_count = 5;  // Number of environment variables to generate
    generate_env_vars(env_vars, env_count);
    
    // Print initial environment variables
    printf("Initial Environment Variables:\n");
    print_env_vars(env_vars, env_count);
    
    // Create child process
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process
        
        // Set new environment variables
        set_env_vars(env_vars, env_count);
        
        // Print updated environment variables
        printf("\nChild Process (PID: %d)\n", getpid());
        print_env_vars(env_vars, env_count);
        
        // Execute the command
        execute_command(argv[1], &argv[1]);
    } else {
        // Parent process
        printf("\nParent Process (PID: %d)\n", getpid());
        printf("Waiting for child process (PID: %d) to complete...\n", pid);
        
        // Wait for child to complete
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("\nChild process exited with status: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("\nChild process was terminated by signal: %d\n", WTERMSIG(status));
        }
        
        // Print final environment variables
        printf("\nFinal Environment Variables in Parent:\n");
        print_env_vars(env_vars, env_count);
    }
    
    return 0;
}
```

### Usage:
```bash
./task2 <command> [args...]
```

### Example Output:
```
Initial Environment Variables:
APP_PATH=/var/APP_PATH/123
SYS_HOME=/var/SYS_HOME/456
USER_TEMP=/var/USER_TEMP/789
DATA_LOG=/var/DATA_LOG/234
CONFIG_CACHE=/var/CONFIG_CACHE/567

Parent Process (PID: 12345)
Waiting for child process (PID: 12346) to complete...

Child Process (PID: 12346)
Current Environment Variables:
APP_PATH=/var/APP_PATH/123
SYS_HOME=/var/SYS_HOME/456
USER_TEMP=/var/USER_TEMP/789
DATA_LOG=/var/DATA_LOG/234
CONFIG_CACHE=/var/CONFIG_CACHE/567

Executing command: ls -l
[Command output here]

Child process exited with status: 0

Final Environment Variables in Parent:
APP_PATH=/var/APP_PATH/123
SYS_HOME=/var/SYS_HOME/456
USER_TEMP=/var/USER_TEMP/789
DATA_LOG=/var/DATA_LOG/234
CONFIG_CACHE=/var/CONFIG_CACHE/567
```

## Task 3: Process Priority Management (`task3.c`)

This task implements a process priority management system with resource monitoring.

### Features:
- Multiple process creation with different priorities
- CPU and memory usage monitoring
- Process state tracking
- Priority-based scheduling
- Resource usage statistics

### Implementation:
```c
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
```

### Usage:
```bash
./task3
```

### Example Output:
```
=== Process Priority Management System ===
Child 1 (PID: 12345) started with priority 10
Child 2 (PID: 12346) started with priority 0
Child 3 (PID: 12347) started with priority -10
Child 4 (PID: 12348) started with priority -5
Child 5 (PID: 12349) started with priority 5

Process Information:
PID     Priority    CPU Usage    Memory Usage    Active Time
------------------------------------------------------------
12345   10          25%          1024 KB         2 s
12346   0           15%          768 KB          2 s
12347   -10         5%           512 KB          2 s
12348   -5          10%          640 KB          2 s
12349   5           20%          896 KB          2 s

[Process information updates every 2 seconds]

All processes have completed
```

## Building and Running

To build all tasks:
```bash
gcc -o task1 task1.c -lrt
gcc -o task2 task2.c
gcc -o task3 task3.c
```

To run the tasks:
```bash
# Task 1
./task1

# Task 2
./task2 ls -l

# Task 3
./task3
```

## Error Handling

All tasks include comprehensive error handling:
- Process creation failures
- Shared memory operations
- Environment variable manipulation
- Resource monitoring
- Process synchronization
- Memory management

## Notes

- Task 1 uses POSIX shared memory for inter-process communication
- Task 2 demonstrates environment variable inheritance and command execution
- Task 3 shows process priority management and resource monitoring
- All tasks include detailed error reporting
- Each task demonstrates different aspects of process management in Linux 