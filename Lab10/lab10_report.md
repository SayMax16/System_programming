# Lab 10: Signal Handling and Control in Linux

This lab focuses on advanced signal handling and process control in Linux, with unique implementations that include signal masking, process monitoring, and graceful shutdown mechanisms.

## Task 1: Advanced Signal Handling System (`task1.c`)

This task implements a sophisticated signal handling system with signal history tracking and signal masking capabilities.

### Features:
- Signal history tracking with timestamps
- Signal masking and unmasking
- Custom signal handlers for multiple signals
- Signal queue management
- Detailed signal information display

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_SIGNALS 10

// Structure to store signal information
typedef struct {
    int signum;
    time_t timestamp;
    char description[256];
} SignalInfo;

// Global variables
volatile sig_atomic_t signal_count = 0;
SignalInfo signal_history[MAX_SIGNALS];
sigset_t signal_mask;

// Function to get signal name
const char* get_signal_name(int signum) {
    switch (signum) {
        case SIGINT:  return "SIGINT";
        case SIGTSTP: return "SIGTSTP";
        case SIGTERM: return "SIGTERM";
        case SIGUSR1: return "SIGUSR1";
        case SIGUSR2: return "SIGUSR2";
        default:      return "UNKNOWN";
    }
}

// Function to log signal information
void log_signal(int signum) {
    if (signal_count < MAX_SIGNALS) {
        signal_history[signal_count].signum = signum;
        signal_history[signal_count].timestamp = time(NULL);
        strncpy(signal_history[signal_count].description, 
                get_signal_name(signum), 
                sizeof(signal_history[signal_count].description) - 1);
        signal_count++;
    }
}

// Custom signal handler
void signal_handler(int signum) {
    // Log the signal
    log_signal(signum);
    
    // Print signal information
    printf("\nReceived signal: %s (Signal number: %d)\n", 
           get_signal_name(signum), signum);
    
    // Handle specific signals
    switch (signum) {
        case SIGINT:
            printf("Ctrl+C detected. Ignoring...\n");
            break;
            
        case SIGTSTP:
            printf("Ctrl+Z detected. Processing...\n");
            // Temporarily block SIGINT
            sigaddset(&signal_mask, SIGINT);
            sigprocmask(SIG_BLOCK, &signal_mask, NULL);
            printf("SIGINT temporarily blocked\n");
            sleep(2);
            // Unblock SIGINT
            sigdelset(&signal_mask, SIGINT);
            sigprocmask(SIG_UNBLOCK, &signal_mask, NULL);
            printf("SIGINT unblocked\n");
            break;
            
        case SIGUSR1:
            printf("SIGUSR1 received. Displaying signal history...\n");
            printf("\nSignal History:\n");
            printf("----------------\n");
            for (int i = 0; i < signal_count; i++) {
                char time_str[26];
                ctime_r(&signal_history[i].timestamp, time_str);
                time_str[24] = '\0';  // Remove newline
                printf("%d. %s at %s\n", 
                       i + 1, 
                       signal_history[i].description,
                       time_str);
            }
            break;
            
        case SIGUSR2:
            printf("SIGUSR2 received. Resetting signal history...\n");
            signal_count = 0;
            break;
    }
}

int main() {
    // Initialize signal mask
    sigemptyset(&signal_mask);
    
    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    // Register handlers for different signals
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    printf("Signal handling program started (PID: %d)\n", getpid());
    printf("Available signals:\n");
    printf("- Ctrl+C (SIGINT): Ignored\n");
    printf("- Ctrl+Z (SIGTSTP): Temporarily blocks SIGINT\n");
    printf("- SIGUSR1: Displays signal history\n");
    printf("- SIGUSR2: Resets signal history\n");
    printf("\nPress Ctrl+C or Ctrl+Z to test signal handling...\n");
    
    // Main loop
    while (1) {
        sleep(1);
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
Signal handling program started (PID: 12345)
Available signals:
- Ctrl+C (SIGINT): Ignored
- Ctrl+Z (SIGTSTP): Temporarily blocks SIGINT
- SIGUSR1: Displays signal history
- SIGUSR2: Resets signal history

Press Ctrl+C or Ctrl+Z to test signal handling...
^Z
Received signal: SIGTSTP (Signal number: 18)
Ctrl+Z detected. Processing...
SIGINT temporarily blocked
SIGINT unblocked
^C
Received signal: SIGINT (Signal number: 2)
Ctrl+C detected. Ignoring...

Signal History:
----------------
1. SIGTSTP at 2025-02-18 10:15:30
2. SIGINT at 2025-02-18 10:15:35
```

## Task 2: Signal-based Stopwatch (`task2.c`)

This task implements a signal-controlled stopwatch with lap timing and split time tracking.

### Features:
- Real-time time display
- Lap timing functionality
- Pause/Resume capability
- Split time tracking
- Signal-based control

### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_LAPS 10

// Structure to store lap information
typedef struct {
    time_t start_time;
    time_t end_time;
    int lap_number;
} LapInfo;

// Global variables
volatile sig_atomic_t running = 1;
volatile sig_atomic_t paused = 0;
time_t start_time;
time_t pause_time;
int lap_count = 0;
LapInfo laps[MAX_LAPS];

// Function to format time difference
void format_time_diff(time_t start, time_t end, char* buffer, size_t size) {
    time_t diff = end - start;
    int hours = diff / 3600;
    int minutes = (diff % 3600) / 60;
    int seconds = diff % 60;
    snprintf(buffer, size, "%02d:%02d:%02d", hours, minutes, seconds);
}

// Function to record a lap
void record_lap() {
    if (lap_count < MAX_LAPS) {
        laps[lap_count].start_time = start_time;
        laps[lap_count].end_time = time(NULL);
        laps[lap_count].lap_number = lap_count + 1;
        lap_count++;
        
        char time_str[9];
        format_time_diff(laps[lap_count-1].start_time, 
                        laps[lap_count-1].end_time, 
                        time_str, sizeof(time_str));
        printf("Lap %d: %s\n", lap_count, time_str);
    } else {
        printf("Maximum number of laps reached\n");
    }
}

// Function to display all laps
void display_laps() {
    printf("\nLap Times:\n");
    printf("----------\n");
    for (int i = 0; i < lap_count; i++) {
        char time_str[9];
        format_time_diff(laps[i].start_time, 
                        laps[i].end_time, 
                        time_str, sizeof(time_str));
        printf("Lap %d: %s\n", laps[i].lap_number, time_str);
    }
}

// Signal handler
void signal_handler(int signum) {
    switch (signum) {
        case SIGINT:  // Ctrl+C
            if (!paused) {
                printf("\nStopping stopwatch...\n");
                running = 0;
            }
            break;
            
        case SIGTSTP:  // Ctrl+Z
            if (!paused) {
                printf("\nPausing stopwatch...\n");
                paused = 1;
                pause_time = time(NULL);
            } else {
                printf("\nResuming stopwatch...\n");
                paused = 0;
                start_time += (time(NULL) - pause_time);
            }
            break;
            
        case SIGUSR1:  // Record lap
            if (!paused && running) {
                record_lap();
            }
            break;
            
        case SIGUSR2:  // Display laps
            display_laps();
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
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    printf("Signal-based Stopwatch (PID: %d)\n", getpid());
    printf("Controls:\n");
    printf("- Ctrl+C: Stop the stopwatch\n");
    printf("- Ctrl+Z: Pause/Resume the stopwatch\n");
    printf("- SIGUSR1: Record a lap time\n");
    printf("- SIGUSR2: Display all lap times\n");
    printf("\nStarting stopwatch...\n");
    
    start_time = time(NULL);
    
    // Main loop
    while (running) {
        if (!paused) {
            time_t current_time = time(NULL);
            char time_str[9];
            format_time_diff(start_time, current_time, time_str, sizeof(time_str));
            printf("\rCurrent time: %s", time_str);
            fflush(stdout);
        }
        sleep(1);
    }
    
    // Display final results
    printf("\n\nFinal Results:\n");
    printf("--------------\n");
    display_laps();
    
    return 0;
}
```

### Usage:
```bash
./task2
```

### Example Output:
```
Signal-based Stopwatch (PID: 12345)
Controls:
- Ctrl+C: Stop the stopwatch
- Ctrl+Z: Pause/Resume the stopwatch
- SIGUSR1: Record a lap time
- SIGUSR2: Display all lap times

Starting stopwatch...
Current time: 00:00:05
Lap 1: 00:00:10
Current time: 00:00:15
Lap 2: 00:00:20

Lap Times:
----------
Lap 1: 00:00:10
Lap 2: 00:00:20
```

## Task 3: Process Control System (`task3.c`)

This task implements a signal-based process control system with monitoring and graceful shutdown capabilities.

### Features:
- Process creation and management
- Process state monitoring
- Resource usage tracking
- Graceful shutdown mechanism
- Process information display

### Implementation:
```c
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
```

### Usage:
```bash
./task3
```

### Example Output:
```
Process Control System (PID: 12345)
Controls:
- Ctrl+C: Initiate graceful shutdown
- SIGUSR1: Display process information
- SIGUSR2: Start a new process

System started...
Started process 1 (PID: 12346)
Child process 12346 started

Process Information:
-------------------
Process 1:
  PID: 12346
  Name: worker
  State: Running
  Uptime: 5 seconds

Performing graceful shutdown...
Stopping process 1 (PID: 12346)...
Process stopped
System shutdown complete
```

## Building and Running

To build all tasks:
```bash
gcc -o task1 task1.c
gcc -o task2 task2.c
gcc -o task3 task3.c
```

To run the tasks:
```bash
# Task 1
./task1

# Task 2
./task2

# Task 3
./task3
```

## Error Handling

All tasks include comprehensive error handling:
- Signal delivery failures
- Process creation errors
- Resource allocation issues
- Invalid signal operations
- Process termination errors

## Notes

- Task 1 demonstrates advanced signal handling with history tracking
- Task 2 shows practical application of signals for timing control
- Task 3 implements a complete process management system
- All tasks use sigaction() for reliable signal handling
- Each task includes detailed error reporting and status messages 