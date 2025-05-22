#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/restaurant_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    char food_item[100];
    int quantity;
    
    // Create socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    // Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    // Order loop
    while (1) {
        // Get food item from user
        printf("Enter food item: ");
        if (fgets(food_item, sizeof(food_item), stdin) == NULL) {
            break;
        }
        food_item[strcspn(food_item, "\n")] = 0;  // Remove newline
        
        // Get quantity from user
        printf("Enter quantity: ");
        if (scanf("%d", &quantity) != 1) {
            break;
        }
        while (getchar() != '\n');  // Clear input buffer
        
        // Format order
        snprintf(buffer, BUFFER_SIZE, "%s,%d", food_item, quantity);
        
        // Send order to server
        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }
        
        // Receive confirmation from server
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
        
        // Ask if user wants to place another order
        printf("Place another order? (y/n): ");
        char choice;
        if (scanf(" %c", &choice) != 1 || choice != 'y') {
            break;
        }
        while (getchar() != '\n');  // Clear input buffer
    }
    
    // Clean up
    close(client_fd);
    
    return 0;
} 