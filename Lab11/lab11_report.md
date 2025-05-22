# Lab 11: Inter-Process Communication and Pipes

**Author:** Saydullo Ismatov (ID: 210057)

## Overview
This lab focuses on implementing various forms of inter-process communication using pipes in Linux, with a focus on reading from and writing to external programs using popen() and pipe-based communication.

## Lab Tasks

### Question 1: Reading from External Program using popen()
This task demonstrates how to read output from external programs like `ls` or `find` using popen().

#### Features:
- Using popen() to execute external commands
- Reading command output into a buffer
- Error handling
- Process management

#### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fp;
    char buffer[1024];
    
    // Execute 'ls -l' command and read its output
    fp = popen("ls -l", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    // Read and print command output
    printf("Output of 'ls -l' command:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    
    // Close the pipe
    pclose(fp);
    return 0;
}
```

#### Compilation and Output:
```bash
$ gcc -o popen1 popen1.c
$ ./popen1
Output of 'ls -l' command:
total 200
-rw-r--r--@ 1 saydulloismatov  staff   9101 May 22 14:50 Lab 11 description.docx
-rw-r--r--@ 1 saydulloismatov  staff  13052 May 22 14:57 assignment2_solutions.md
-rw-r--r--@ 1 saydulloismatov  staff    550 May 22 15:02 consumer.c
-rw-r--r--@ 1 saydulloismatov  staff   8507 May 22 15:01 lab11_solutions.md
-rw-r--r--@ 1 saydulloismatov  staff   1030 May 22 15:02 pipe1.c
-rw-r--r--@ 1 saydulloismatov  staff   1774 May 22 15:02 pipe2.c
-rwxr-xr-x@ 1 saydulloismatov  staff  33768 May 22 15:03 popen1
-rw-r--r--@ 1 saydulloismatov  staff    514 May 22 15:01 popen1.c
-rw-r--r--@ 1 saydulloismatov  staff    764 May 22 15:02 popen2.c
-rw-r--r--@ 1 saydulloismatov  staff    728 May 22 15:02 producer.c
```

### Question 2: Writing to External Program using popen()
This task demonstrates how to write input to external programs like `sort` or `wc` using popen().

#### Features:
- Using popen() to write to external commands
- Data processing through external programs
- Error handling
- Process management

#### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    FILE *fp;
    char buffer[1024];
    
    // Open sort command for writing
    fp = popen("sort", "w");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    // Write data to sort
    fprintf(fp, "banana\n");
    fprintf(fp, "apple\n");
    fprintf(fp, "cherry\n");
    fprintf(fp, "date\n");
    
    // Close the pipe
    pclose(fp);
    
    // Now read the sorted output
    fp = popen("sort", "r");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    
    printf("Sorted output:\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }
    
    pclose(fp);
    return 0;
}
```

#### Compilation and Output:
```bash
$ gcc -o popen2 popen2.c
$ ./popen2
apple
banana
cherry
date
Sorted output:
```

### Question 3: Single Process Communication using Pipe
This task demonstrates basic pipe communication within a single process using fork().

#### Features:
- Pipe creation using pipe()
- Parent-child process communication
- Basic message passing
- Error handling

#### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[100];
    
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
        // Child process
        close(pipefd[1]);  // Close write end
        
        // Read message from parent
        read(pipefd[0], buffer, sizeof(buffer));
        printf("Child received: %s\n", buffer);
        
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        close(pipefd[0]);  // Close read end
        
        // Send message to child
        const char* message = "Hello from parent!";
        write(pipefd[1], message, strlen(message) + 1);
        
        close(pipefd[1]);
        wait(NULL);
    }
    
    return 0;
}
```

#### Compilation and Output:
```bash
$ gcc -o pipe1 pipe1.c
$ ./pipe1
Child received: Hello from parent!
```

### Question 4: Multi-Process Pipe Communication
This task demonstrates communication between a parent process and two child processes using pipes.

#### Features:
- Multiple pipes for process communication
- Parent process creating two child processes
- Bidirectional communication
- Error handling

#### Implementation:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;
    char buffer[100];
    
    // Create pipes
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
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
        // First child process
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        
        // Read from parent
        read(pipe1[0], buffer, sizeof(buffer));
        printf("Child 1 received: %s\n", buffer);
        
        close(pipe1[0]);
        exit(EXIT_SUCCESS);
    }
    
    // Create second child process
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    
    if (pid2 == 0) {
        // Second child process
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[1]);
        
        // Read from parent
        read(pipe2[0], buffer, sizeof(buffer));
        printf("Child 2 received: %s\n", buffer);
        
        close(pipe2[0]);
        exit(EXIT_SUCCESS);
    }
    
    // Parent process
    close(pipe1[0]);
    close(pipe2[0]);
    
    // Send message to first child
    const char* message1 = "Hello to Child 1!";
    write(pipe1[1], message1, strlen(message1) + 1);
    
    // Send message to second child
    const char* message2 = "Hello to Child 2!";
    write(pipe2[1], message2, strlen(message2) + 1);
    
    close(pipe1[1]);
    close(pipe2[1]);
    
    // Wait for children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    
    return 0;
}
```

#### Compilation and Output:
```bash
$ gcc -o pipe2 pipe2.c
$ ./pipe2
Child 1 received: Hello to Child 1!
Child 2 received: Hello to Child 2!
```

### Question 5: Pipe between Different Programs
This task demonstrates communication between two different programs (producer and consumer) using pipes.

#### Features:
- Separate producer and consumer programs
- Named pipe (FIFO) creation and usage
- Data production and consumption
- Error handling

#### Implementation (Producer):
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_NAME "/tmp/myfifo"

int main() {
    int fd;
    char buffer[100];
    
    // Create FIFO
    mkfifo(FIFO_NAME, 0666);
    
    // Open FIFO for writing
    fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    // Generate and send data
    for (int i = 1; i <= 5; i++) {
        snprintf(buffer, sizeof(buffer), "Data %d", i);
        write(fd, buffer, strlen(buffer) + 1);
        printf("Producer sent: %s\n", buffer);
        sleep(1);
    }
    
    close(fd);
    unlink(FIFO_NAME);
    return 0;
}
```

#### Implementation (Consumer):
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_NAME "/tmp/myfifo"

int main() {
    int fd;
    char buffer[100];
    
    // Open FIFO for reading
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    // Read and process data
    while (read(fd, buffer, sizeof(buffer)) > 0) {
        printf("Consumer received: %s\n", buffer);
    }
    
    close(fd);
    return 0;
}
```

#### Compilation and Output:
```bash
# Terminal 1 (Consumer)
$ gcc -o consumer consumer.c
$ ./consumer

# Terminal 2 (Producer)
$ gcc -o producer producer.c
$ ./producer
Producer sent: Data 1
Consumer received: Data 1
Producer sent: Data 2
Consumer received: Data 2
Producer sent: Data 3
Consumer received: Data 3
Producer sent: Data 4
Consumer received: Data 4
Producer sent: Data 5
Consumer received: Data 5
``` 