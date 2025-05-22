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