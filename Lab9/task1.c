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