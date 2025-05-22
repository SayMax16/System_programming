# Assignment 2: Inter-Process Communication Solutions

**Author:** Saydullo Ismatov (ID: 210057)

## Overview
This assignment focuses on implementing various forms of inter-process communication using pipes in Linux. The tasks cover different aspects of process communication, including basic pipes, named pipes, and data transformation between processes.

## Task 1: Two-Child Communication using Pipe

### Description
Write a C program where a parent process creates two child processes:
- Child 1 sends a message to Child 2 using a pipe
- Child 2 receives the message and prints it
- Implement proper error handling and resource management

### Implementation
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid1, pid2;
    char buffer[100];
    
    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create first child process
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid1 == 0) {
        // Child 1 process
        close(pipefd[0]);  // Close read end
        
        // Send message to Child 2
        const char* message = "Hello from Child 1!";
        write(pipefd[1], message, strlen(message) + 1);
        printf("Child 1 sent: %s\n", message);
        
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    
    // Create second child process
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid2 == 0) {
        // Child 2 process
        close(pipefd[1]);  // Close write end
        
        // Read message from Child 1
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child 2 received: %s\n", buffer);
        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    
    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Wait for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    return 0;
}
```

### Output
```
Child 1 sent: Hello from Child 1!
Child 2 received: Hello from Child 1!
```

## Task 2: popen with fread and fwrite

### Description
Create a C program that:
- Uses popen() with find command to locate .txt files
- Stores output in a buffer using fread()
- Uses popen() with wc command to count lines and characters
- Uses popen() with od command to display output in octal format

### Implementation
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fp;
    char buffer[1024];
    size_t bytes_read;
    
    // First command: find .txt files
    char command1[] = "find . -type f -name \"*.txt\"";
    fp = popen(command1, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    printf("Found .txt files:\n");
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        fwrite(buffer, 1, bytes_read, stdout);
    }
    pclose(fp);
    
    // Second command: count lines and characters
    char command2[] = "cat abc.txt | wc -l && cat abc.txt | wc -c";
    fp = popen(command2, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    printf("\nLine and character count:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    
    // Third command: display in octal format
    char command3[] = "od -c abc.txt";
    fp = popen(command3, "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    printf("\nOctal format:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    pclose(fp);
    
    return 0;
}
```

### Output
```
Found .txt files:
./abc.txt

Line and character count:
       3
     109

Octal format:
0000000    T   h   i   s       i   s       a       t   e   s   t       f
0000020    i   l   e   .  \n   I   t       h   a   s       m   u   l   t
0000040    i   p   l   e       l   i   n   e   s   .  \n   E   a   c   h
0000060        l   i   n   e       c   o   n   t   a   i   n   s       s
0000100    o   m   e       t   e   x   t   .  \n   T   h   e       f   i
0000120    l   e       w   i   l   l       b   e       u   s   e   d    
0000140    f   o   r       t   e   s   t   i   n   g   .                
0000155
```

## Task 3: Producer-Consumer using Pipe

### Description
Write a C program using fork to:
- Create a producer that writes 5 random numbers to a pipe
- Consumer reads the numbers and calculates the sum
- Use a loop for writing and reading

### Implementation
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main() {
    int pipefd[2];
    pid_t pid;
    int numbers[5];
    int sum = 0;
    
    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // Consumer process
        close(pipefd[1]);  // Close write end
        
        // Read numbers and calculate sum
        for (int i = 0; i < 5; i++) {
            read(pipefd[0], &numbers[i], sizeof(int));
            sum += numbers[i];
            printf("Consumer received: %d\n", numbers[i]);
        }
        
        printf("Sum of numbers: %d\n", sum);
        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // Producer process
        close(pipefd[0]);  // Close read end
        
        // Generate and send random numbers
        srand(time(NULL));
        for (int i = 0; i < 5; i++) {
            numbers[i] = rand() % 100;
            write(pipefd[1], &numbers[i], sizeof(int));
            printf("Producer sent: %d\n", numbers[i]);
        }
        
        close(pipefd[1]);
        wait(NULL);
    }
    
    return 0;
}
```

### Output
```
Producer sent: 0
Producer sent: 27
Producer sent: 9
Producer sent: 7
Producer sent: 50
Consumer received: 0
Consumer received: 27
Consumer received: 9
Consumer received: 7
Consumer received: 50
Sum of numbers: 93
```

## Task 4: Pipe Between Parent and Child

### Description
Create a program where:
- Parent sends an array of integers to child using pipe
- Child calculates and returns the average using pipe
- Use two pipes for full-duplex communication

### Implementation
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define ARRAY_SIZE 5

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid;
    int numbers[ARRAY_SIZE] = {10, 20, 30, 40, 50};
    float average;
    
    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {
        // Child process
        close(pipe1[1]);  // Close write end of first pipe
        close(pipe2[0]);  // Close read end of second pipe
        
        int received_numbers[ARRAY_SIZE];
        float sum = 0;
        
        // Read numbers from parent
        read(pipe1[0], received_numbers, sizeof(received_numbers));
        printf("Child received numbers: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", received_numbers[i]);
            sum += received_numbers[i];
        }
        printf("\n");
        
        // Calculate and send average
        average = sum / ARRAY_SIZE;
        write(pipe2[1], &average, sizeof(average));
        printf("Child calculated average: %.2f\n", average);
        
        close(pipe1[0]);
        close(pipe2[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipe1[0]);  // Close read end of first pipe
        close(pipe2[1]);  // Close write end of second pipe
        
        // Send numbers to child
        printf("Parent sending numbers: ");
        for (int i = 0; i < ARRAY_SIZE; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");
        write(pipe1[1], numbers, sizeof(numbers));
        
        // Read average from child
        read(pipe2[0], &average, sizeof(average));
        printf("Parent received average: %.2f\n", average);
        
        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL);
    }
    
    return 0;
}
```

### Output
```
Parent sending numbers: 10 20 30 40 50 
Child received numbers: 10 20 30 40 50 
Child calculated average: 30.00
Parent received average: 30.00
```

## Task 5: Uppercase Message Transfer between Children

### Description
- Parent creates a pipe and two child processes
- One child writes a string message into the pipe
- Other child reads the message, converts it to uppercase, and displays it
- Ensure correct closing of file descriptors

### Implementation
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

int main() {
    int pipefd[2];
    pid_t pid1, pid2;
    char buffer[100];
    
    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    // Create first child process
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid1 == 0) {
        // First child process (writer)
        close(pipefd[0]);  // Close read end
        
        // Write message to pipe
        const char* message = "Hello from Child 1!";
        write(pipefd[1], message, strlen(message) + 1);
        printf("Child 1 wrote: %s\n", message);
        
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    }
    
    // Create second child process
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid2 == 0) {
        // Second child process (reader)
        close(pipefd[1]);  // Close write end
        
        // Read message and convert to uppercase
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child 2 received: %s\n", buffer);
        
        // Convert to uppercase
        for (int i = 0; buffer[i]; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        printf("Child 2 converted to uppercase: %s\n", buffer);
        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
    
    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Wait for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    return 0;
}
```

### Output
```
Child 1 wrote: Hello from Child 1!
Child 2 received: Hello from Child 1!
Child 2 converted to uppercase: HELLO FROM CHILD 1!
```

## Task 6: Named Pipe Producer-Consumer

### Description
Create three separate C programs:
- One to create a named pipe using mkfifo()
- A Producer Program that continuously reads strings from user input and writes to the named pipe
- A Consumer Program that reads strings using fgets() and displays them
- Use separate terminals for producer and consumer programs

### Implementation

#### FIFO Creator (assignment_task6_fifo.c)
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_NAME "/tmp/myfifo"

int main() {
    // Create named pipe
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }
    
    printf("Named pipe created: %s\n", FIFO_NAME);
    return 0;
}
```

#### Producer (assignment_task6_producer.c)
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/myfifo"
#define BUFFER_SIZE 100

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    // Open FIFO for writing
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    printf("Producer started. Type 'exit' to quit.\n");
    
    while (1) {
        printf("Enter message: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Remove newline
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        // Write to FIFO
        if (write(fd, buffer, strlen(buffer) + 1) == -1) {
            perror("write");
            break;
        }
    }
    
    close(fd);
    return 0;
}
```

#### Consumer (assignment_task6_consumer.c)
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "/tmp/myfifo"
#define BUFFER_SIZE 100

int main() {
    int fd;
    char buffer[BUFFER_SIZE];
    
    // Open FIFO for reading
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    printf("Consumer started. Waiting for messages...\n");
    
    while (1) {
        // Read from FIFO
        ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            break;
        }
        
        printf("Received: %s\n", buffer);
    }
    
    close(fd);
    return 0;
}
```

### Output
To run Task 6, follow these steps:

1. Create the FIFO:
```bash
./fifo
```
Output:
```
Named pipe created: /tmp/myfifo
```

2. Start the consumer in one terminal:
```bash
./consumer
```
Output:
```
Consumer started. Waiting for messages...
```

3. Start the producer in another terminal:
```bash
./producer
```
Output:
```
Producer started. Type 'exit' to quit.
Enter message: Hello
Enter message: World
Enter message: exit
```

The consumer terminal will show:
```
Received: Hello
Received: World
```

## Conclusion
All tasks have been successfully implemented and tested. Each program demonstrates different aspects of inter-process communication using pipes in Linux, from basic pipe communication to more complex scenarios involving multiple processes and named pipes. 