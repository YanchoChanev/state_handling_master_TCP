#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"

// TCP Client Task
void tcpClientTask(void *pvParameters) {
    TCPConfig_t *config = (TCPConfig_t *)pvParameters;

    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[config->buffer_size];
    ssize_t bytes_received = 0;

    printf("Client Configuration: IP=%s, PORT=%d, BUFFER=%d\n",
           config->server_ip, config->server_port, config->buffer_size);

    while (1) {
        // Create socket
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            perror("[Client] Socket creation failed");
            vTaskDelay(pdMS_TO_TICKS(5000)); // Retry after 5 seconds
            continue;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(config->server_port);
        server_addr.sin_addr.s_addr = inet_addr(config->server_ip);

        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("[Client] Connect failed");
            close(client_fd);
            vTaskDelay(pdMS_TO_TICKS(5000)); // Retry after 5 seconds
            continue;
        }

        printf("[Client] Connected to server at %s:%d\n", config->server_ip, config->server_port);

        // Communication Loop
        while (1) {
            // Prepare message
            MasterStates currentStatus = getCurrentState();
            if (currentStatus == ERROR) {
                snprintf(config->message, sizeof(config->message), "ID=%d;DATA=%d", 5002, ERROR);
            } else {
                snprintf(config->message, sizeof(config->message), "ID=%d;DATA=%d", 5002, MAX_STATE_MASTER);
            }

            // Send message
            ssize_t sent_bytes = send(client_fd, config->message, strlen(config->message), 0);
            if (sent_bytes < 0) {
                if (errno == EINTR) {
                    printf("[Client] Send interrupted, retrying...\n");
                    continue; // Retry the send operation
                }
                perror("[Client] Send failed");
                break; // Exit inner loop and reconnect
            }

            printf("[Client] Sent: %s\n", config->message);

            // Receive response
            bytes_received = recv(client_fd, buffer, config->buffer_size - 1, 0);
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0'; // Ensure buffer is null-terminated
                printf("[Client] Received: %s\n", buffer);

                uint8_t state = 0;
                if (sscanf(buffer, "STATUS=%hhu", &state) == 1) {
                    printf("[Client] Parsed STATUS: %d\n", state);
                    // if (xQueueSend(config->xQueue, &state, pdMS_TO_TICKS(1000)) == pdPASS) {
                    //     printf("[Client] Sent to queue: state=%d\n", state);
                    // } else {
                    //     printf("[Client] Failed to send to queue (Queue Full)\n");
                    // }
                    stateDispatcher(state);
                } else {
                    printf("[Client] Failed to parse buffer: %s\n", buffer);
                }
            } 
            else if (bytes_received == 0) {
                // Connection closed by server
                printf("[Client] Server closed the connection.\n");
                break;
            } 
            else {
                if (errno == EINTR) {
                    printf("[Client] Receive interrupted, retrying...\n");
                    continue; // Retry the recv operation
                }
                perror("[Client] Error while receiving data");
                break; // Break inner loop and reconnect
            }

            vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay before next iteration
        }

        // Clean up and reconnect
        close(client_fd);
        printf("[Client] Disconnected. Reconnecting in 5 seconds...\n");
        vTaskDelay(pdMS_TO_TICKS(5000)); // Wait before reconnecting
    }

    vTaskDelete(NULL);
}
