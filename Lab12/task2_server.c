#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "/tmp/restaurant_socket"
#define BUFFER_SIZE 1024

// Structure for order information
typedef struct {
    char food_item[100];
    int quantity;
    time_t order_time;
} Order;

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);
    
    // Create socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Remove existing socket file if it exists
    unlink(SOCKET_PATH);
    
    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is ready to accept orders...\n");
    
    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Customer connected\n");
    
    // Order processing loop
    while (1) {
        // Receive order from client
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        
        // Parse order
        Order order;
        if (sscanf(buffer, "%[^,],%d", order.food_item, &order.quantity) != 2) {
            printf("Invalid order format received\n");
            continue;
        }
        
        order.order_time = time(NULL);
        
        // Process order
        printf("Received order: %s x %d\n", order.food_item, order.quantity);
        
        // Send confirmation to client
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE, "Order Confirmation: %s x %d has been received.\n",
                order.food_item, order.quantity);
        send(client_fd, response, strlen(response), 0);
        
        // Simulate order processing time
        sleep(1);
    }
    
    printf("Customer left the restaurant\n");
    printf("Server closed\n");
    
    // Clean up
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
    
    return 0;
} 