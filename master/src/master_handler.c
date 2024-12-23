#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "types.h"
#include "master_handler.h"
#include "master_state_machine.h"
#include "master_tcp_comm_server.h"
#include "master_tcp_comm_client.h"
#include "logger.h"

// Handles master communication tasks.
// void vMasterCommHandler(void *args) {
//     logMessage(LOG_LEVEL_INFO, "MasterHandler", "Master communication handler started");
//     QueueMessage data;

//     while (1) {
//         // printf("Master communication handler\n");
//         if (reciveMsgMaster(&data) != RET_OK) {
//             logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to receive message");
//         }

//         logMessageFormatted(LOG_LEVEL_DEBUG, "MasterHandler", "Message ID = %d, message state = %d", data.msg_id, data.state);
//         // printf("Message ID = %d, message state = %d\n", data.msg_id, data.state);
//         if (stateDispatcher(data) != RET_OK) {
//             logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to dispatch state");
//         }

//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
//     // vTaskDelay(pdMS_TO_TICKS(100));
// }

//Handles master status check tasks.
// void vMasterStatusCheckHandler(void *args) {
//     logMessage(LOG_LEVEL_INFO, "MasterHandler", "Master status check handler started");
//     QueueMessage data = {SLAVE_CURRENT_STATUS, 0};

//     while (1) {
//         if (sendMsgMaster(&data) != RET_OK) {
//             logMessage(LOG_LEVEL_ERROR, "MasterHandler", "Failed to send message");
//         }

//         logMessage(LOG_LEVEL_DEBUG, "MasterHandler", "Sent status check message to slave");
//         vTaskDelay(pdMS_TO_TICKS(90));
//     }
//     // vTaskDelay(pdMS_TO_TICKS(100));
// }

void vCreateTCPServer(void *args){
    TCPConfig_t *configServer = pvPortMalloc(sizeof(TCPConfig_t));
    if (configServer == NULL) {
        perror("Failed to allocate memory for server config");
        return;
    }

    strncpy(configServer->server_ip, MASTER_SERVER_IP, sizeof(configServer->server_ip) - 1);
    configServer->server_ip[sizeof(configServer->server_ip) - 1] = '\0';
    configServer->server_port = MASTER_SERVER_PORT;
    configServer->buffer_size = MASTER_SERVER_SIZE;

    tcpServerTask((void *)configServer);
}

void vCreateTCPClient(void *args){
    TCPConfig_t configClient;
    configClient.xQueue = (QueueHandle_t)args;
    // if (configClient == NULL) {
    //     perror("Failed to allocate memory for client config");
    //     return;
    // }

    strncpy(configClient.server_ip, SLAVE_SERVER_IP, sizeof(configClient.server_ip) - 1);
    configClient.server_ip[sizeof(configClient.server_ip) - 1] = '\0';
    configClient.server_port = SLAVE_SERVER_PORT;
    configClient.buffer_size = SLAVE_SERVER_SIZE;

    tcpClientTask((void *)&configClient);
}

// void vRunStateMachine(void* args){
//     stateDispatcher(args);
// }