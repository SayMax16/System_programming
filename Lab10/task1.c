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