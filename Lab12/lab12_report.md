# Lab 12: Interprocess Communication using UNIX Domain Sockets

**Author:** Saydullo Ismatov (ID: 210057)

## Overview
This lab focuses on implementing local interprocess communication (IPC) using UNIX domain sockets in C programming, with unique implementations that include a chat application and a restaurant order system.

## Task 1: Chat Application
This task implements a client-server chat application using UNIX domain sockets with features like message echoing and graceful connection termination.

### Features:
- Stream-based UNIX domain socket communication
- Bidirectional message exchange
- Graceful connection termination
- Error handling and resource cleanup
- Socket file management

### Implementation:

#### Server (`task1_server.c`):
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/chat_socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);
    
    
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    
    unlink(SOCKET_PATH);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    

    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server started, waiting for client connection...\n");
    

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Client connected\n");
    
    // Chat loop
    while (1) {
        // Receive message from client
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("Client: %s", buffer);
        
        // Check for "bye" message
        if (strcmp(buffer, "bye\n") == 0) {
            break;
        }
        
        // Send response back to client
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE, "Server Response: %s", buffer);
        send(client_fd, response, strlen(response), 0);
    }
    
    printf("Closing connection\n");
    
    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    
    return 0;
}
```

#### Client (`task1_client.c`):
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/chat_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    
    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    // Chat loop
    while (1) {
        // Get message from user
        printf("Enter message: ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Send message to server
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }
        
        // Check for "bye" message
        if (strcmp(buffer, "bye\n") == 0) {
            break;
        }
        
        // Receive response from server
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    
    printf("Connection closed.\n");
    
    // Clean up
    close(client_fd);
    
    return 0;
}
```

### Example Output:
```
# Server Output:
Server started, waiting for client connection...
Client connected
Client: Hello, Server!
Client: bye
Closing connection

# Client Output:
Enter message: Hello, Server!
Server Response: Hello, Server!
Enter message: bye
Server Response: bye
Connection closed.
```

## Task 2: Restaurant Order System
This task implements a restaurant order system using UNIX domain sockets, where clients can place food orders and receive confirmations.

### Features:
- Structured order data transmission
- Order validation and processing
- Timestamp tracking
- Multiple order support
- Interactive order placement

### Implementation:
See `task2_server.c` and `task2_client.c` in the lab directory.

### Example Output:
```
# Server Output:
Server is ready to accept orders...
Customer connected
Received order: Pizza x 2
Received order: Salad x 1
Customer left the restaurant
Server closed

# Client Output:
Enter food item: Pizza
Enter quantity: 2
Order Confirmation: Pizza x 2 has been received.
Place another order? (y/n): y
Enter food item: Salad
Enter quantity: 1
Order Confirmation: Salad x 1 has been received.
Place another order? (y/n): n
```

## Building and Running

To build all tasks:
```bash
# Task 1
gcc -o server task1_server.c
gcc -o client task1_client.c

# Task 2
gcc -o restaurant_server task2_server.c
gcc -o restaurant_client task2_client.c
```

## Error Handling

All tasks include comprehensive error handling:
- Socket creation failures
- Connection errors
- Data transmission issues
- Resource cleanup
- Invalid input handling

## Notes

- Task 1 demonstrates basic UNIX domain socket communication
- Task 2 shows practical application with structured data
- Both tasks include proper resource management
- Each task demonstrates different aspects of IPC in Linux
- The implementations use stream sockets (SOCK_STREAM) for reliable communication 
