# LAB2: Process Management in Unix/Linux - Report
## Student: Saydullo Ismatov (210057)

This report demonstrates various aspects of process management in Unix/Linux systems using system calls like fork(), exec(), and wait().

## 1. Basic Process Creation and Command Execution

### Code Example 1: fork_exec_wait.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid, child_pid;
    int status;

    printf("Parent process ID: %d\n", getpid());

    pid = fork();

    if (pid < 0) {

        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {
        printf("Child process ID: %d, Parent ID: %d\n", getpid(), getppid());
        

        execlp("ls", "ls", "-l", NULL);
        
        perror("Exec failed");
        exit(1);
    } else {
        printf("Parent: Created child with PID: %d\n", pid);
        
        child_pid = wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Parent: Child %d terminated with exit status %d\n", 
                   child_pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Parent: Child %d terminated by signal %d\n", 
                   child_pid, WTERMSIG(status));
        }
    }

    return 0;
}
```

### Explanation:
This program demonstrates the basic fork-exec-wait pattern:
1. Creates a child process using fork()
2. Child process executes "ls -l" using execlp()
3. Parent process waits for child to complete using wait()
4. Parent reports child's exit status

### Actual Output:
```
Parent process ID: 11601
Parent: Created child with PID: 11602
Child process ID: 11602, Parent ID: 11601
total 200
drwxr-xr-x   5 saydulloismatov  staff    160 May 22 13:38 LAB1
drwxr-xr-x  15 saydulloismatov  staff    480 May 22 14:09 LAB2
drwxr-xr-x@  9 saydulloismatov  staff    288 May 22 13:58 Lab3
drwxr-xr-x@  9 saydulloismatov  staff    288 May 21 22:49 Lab4
drwxr-xr-x@  7 saydulloismatov  staff    224 May 21 22:51 Lab5
drwxr-xr-x@  9 saydulloismatov  staff    288 May 22 14:04 Lab6
-rwxr-xr-x@  1 saydulloismatov  staff  33728 May 22 14:09 fork_exec_wait
-rw-r--r--@  1 saydulloismatov  staff  65493 May 22 13:42 labsysprog2.pdf
Parent: Child 11602 terminated with exit status 0
```

## 2. Using Different exec() Variants

### Code Example 2: exec_variants.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {

        printf("Child process (PID: %d) will execute 'ls -la /tmp'\n", getpid());
        
        char *args[] = {"ls", "-la", "/tmp", NULL};
        execvp("ls", args);
        perror("execvp failed");
        exit(1);
    } else {
        printf("Parent process (PID: %d) created child (PID: %d)\n", getpid(), pid);
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Child exited with status: %d\n", WEXITSTATUS(status));
        } else {
            printf("Child terminated abnormally\n");
        }
    }

    return 0;
}
```

### Explanation:
This program demonstrates using execvp() instead of execlp():
1. Uses an array of arguments for the command
2. Shows how to pass multiple arguments to a command
3. Demonstrates a different variant of the exec() family

### Actual Output:
```
Parent process (PID: 11832) created child (PID: 11835)
Child process (PID: 11835) will execute 'ls -la /tmp'
lrwxr-xr-x@ 1 root  wheel  11 Dec  7 13:11 /tmp -> private/tmp
Child exited with status: 0
```

## 3. Managing Multiple Child Processes

### Code Example 3: multiple_children.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_CHILDREN 3

int main() {
    pid_t pid[NUM_CHILDREN];
    int i, status;

    printf("Parent process (PID: %d) will create %d children\n", getpid(), NUM_CHILDREN);

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid[i] = fork();
        
        if (pid[i] < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid[i] == 0) {
            printf("Child %d (PID: %d) created by parent (PID: %d)\n", 
                   i + 1, getpid(), getppid());
        
            sleep(NUM_CHILDREN - i);
            
            printf("Child %d (PID: %d) is exiting\n", i + 1, getpid());
            exit(i + 1);  
        } else {
            printf("Parent: Created child %d (PID: %d)\n", i + 1, pid[i]);
        }
    }

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid_t terminated_pid = wait(&status);
        

        int child_index = -1;
        for (int j = 0; j < NUM_CHILDREN; j++) {
            if (pid[j] == terminated_pid) {
                child_index = j;
                break;
            }
        }
        
        if (WIFEXITED(status)) {
            printf("Parent: Child %d (PID: %d) exited with status %d\n", 
                   child_index + 1, terminated_pid, WEXITSTATUS(status));
        } else {
            printf("Parent: Child %d (PID: %d) terminated abnormally\n", 
                   child_index + 1, terminated_pid);
        }
    }

    printf("Parent: All children have terminated\n");
    return 0;
}
```

### Explanation:
This program demonstrates managing multiple child processes:
1. Creates 3 child processes in a loop
2. Each child sleeps for a different duration
3. Parent waits for all children to complete
4. Shows how to track multiple child processes

### Actual Output:
```
Parent process (PID: 11848) will create 3 children
Parent: Created child 1 (PID: 11849)
Parent: Created child 2 (PID: 11850)
Child 1 (PID: 11849) created by parent (PID: 11848)
Child 2 (PID: 11850) created by parent (PID: 11848)
Parent: Created child 3 (PID: 11851)
Child 3 (PID: 11851) created by parent (PID: 11848)
Child 3 (PID: 11851) is exiting
Parent: Child 3 (PID: 11851) exited with status 3
Child 2 (PID: 11850) is exiting
Parent: Child 2 (PID: 11850) exited with status 2
Child 1 (PID: 11849) is exiting
Parent: Child 1 (PID: 11849) exited with status 1
Parent: All children have terminated
```

## 4. Process Chain (Parent → Child → Grandchild)

### Code Example 4: process_chain.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid1, pid2;
    int status;

    printf("Parent process (PID: %d) started\n", getpid());

    pid1 = fork();

    if (pid1 < 0) {
        perror("First fork failed");
        exit(1);
    } else if (pid1 == 0) {
        printf("Child (level 1, PID: %d, Parent PID: %d) started\n", 
               getpid(), getppid());
        
        pid2 = fork();
        
        if (pid2 < 0) {
            perror("Second fork failed");
            exit(2);
        } else if (pid2 == 0) {
            printf("Grandchild (level 2, PID: %d, Parent PID: %d) started\n", 
                   getpid(), getppid());
            
            sleep(2);
            
            printf("Grandchild (level 2, PID: %d) exiting\n", getpid());
            exit(3);
        } else {
            printf("Child (level 1, PID: %d) created grandchild (PID: %d)\n", 
                   getpid(), pid2);
            

            wait(&status);
            
            if (WIFEXITED(status)) {
                printf("Child (level 1, PID: %d): Grandchild exited with status %d\n", 
                       getpid(), WEXITSTATUS(status));
            }
            
            printf("Child (level 1, PID: %d) exiting\n", getpid());
            exit(4);
        }
    } else {
        printf("Parent (PID: %d) created child (PID: %d)\n", getpid(), pid1);
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Parent (PID: %d): Child exited with status %d\n", 
                   getpid(), WEXITSTATUS(status));
        }
        
        printf("Parent (PID: %d) exiting\n", getpid());
    }

    return 0;
}
```

### Explanation:
This program demonstrates a process hierarchy:
1. Parent creates a child process
2. Child creates its own child (grandchild)
3. Shows how processes can create their own children
4. Demonstrates waiting for children at each level

### Actual Output:
```
Parent process (PID: 11865) started
Parent (PID: 11865) created child (PID: 11866)
Child (level 1, PID: 11866, Parent PID: 11865) started
Child (level 1, PID: 11866) created grandchild (PID: 11867)
Grandchild (level 2, PID: 11867, Parent PID: 11866) started
Grandchild (level 2, PID: 11867) exiting
Child (level 1, PID: 11866): Grandchild exited with status 3
Child (level 1, PID: 11866) exiting
Parent (PID: 11865): Child exited with status 4
Parent (PID: 11865) exiting
```

## 5. Signal Handling Between Processes

### Code Example 5: signal_handling.c
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

pid_t child_pid = -1;

void parent_signal_handler(int signum) {
    printf("Parent (PID: %d) received signal: %s (%d)\n", 
           getpid(), strsignal(signum), signum);
    
    if (child_pid > 0) {
        printf("Parent is sending SIGTERM to child (PID: %d)\n", child_pid);
        kill(child_pid, SIGTERM);
    }
}

void child_signal_handler(int signum) {
    printf("Child (PID: %d) received signal: %s (%d)\n", 
           getpid(), strsignal(signum), signum);
    
    if (signum == SIGTERM) {
        printf("Child (PID: %d) is terminating due to SIGTERM\n", getpid());
        exit(0);
    }
}

int main() {
    pid_t pid;
    int status;
    struct sigaction sa_parent, sa_child;

    printf("Parent process (PID: %d) started\n", getpid());

    memset(&sa_parent, 0, sizeof(sa_parent));
    sa_parent.sa_handler = &parent_signal_handler;
    sigaction(SIGINT, &sa_parent, NULL);


    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    } else if (pid == 0) {

        printf("Child process (PID: %d, Parent PID: %d) started\n", 
               getpid(), getppid());
        
        memset(&sa_child, 0, sizeof(sa_child));
        sa_child.sa_handler = &child_signal_handler;
        sigaction(SIGTERM, &sa_child, NULL);
        
        printf("Child (PID: %d) running. Press Ctrl+C to test signal handling\n", getpid());
        
        while (1) {
            sleep(1);
        }
        

        exit(0);
    } else {

        child_pid = pid;  
        printf("Parent (PID: %d) created child (PID: %d)\n", getpid(), pid);
        printf("Parent waiting for child. Press Ctrl+C to test signal handling\n");
        
        wait(&status);
        
        if (WIFEXITED(status)) {
            printf("Parent: Child exited with status %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Parent: Child terminated by signal %d (%s)\n", 
                   WTERMSIG(status), strsignal(WTERMSIG(status)));
        }
        
        printf("Parent (PID: %d) exiting\n", getpid());
    }

    return 0;
}
```

### Explanation:
This program demonstrates signal handling between processes:
1. Parent process handles SIGINT (Ctrl+C)
2. Child process handles SIGTERM
3. Parent can send signals to child using kill()
4. Shows how to properly handle process termination

### Actual Output (when Ctrl+C is pressed):
```
Parent process (PID: 11883) started
Parent (PID: 11883) created child (PID: 11884)
Parent waiting for child. Press Ctrl+C to test signal handling
Child process (PID: 11884, Parent PID: 11883) started
Child (PID: 11884) running. Press Ctrl+C to test signal handling
^C
Parent (PID: 11883) received signal: Interrupt: 2 (2)
Parent (PID: 11884) received signal: Interrupt: 2 (2)
Parent is sending SIGTERM to child (PID: 11884)
Parent: Child terminated by signal 2 (Interrupt: 2)
Child (PID: 11884) received signal: Terminated: 15 (15)
Child (PID: 11884) is terminating due to SIGTERM
Parent (PID: 11883) exiting
```

## Conclusion

These examples demonstrate the fundamental concepts of process management in Unix/Linux systems:
1. Process creation using fork()
2. Program execution using exec() family functions
3. Process synchronization using wait()
4. Process hierarchies and relationships
5. Signal handling between processes

Each example builds upon the previous ones, showing increasingly complex process management scenarios. The code includes proper error handling and demonstrates best practices for process management in Unix/Linux systems.

The actual outputs show how these programs behave in a real Unix/Linux environment, with real process IDs and system responses. The outputs demonstrate the parent-child relationships, process creation and termination, and signal handling between processes. 