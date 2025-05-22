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