/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h> // Include pthread.h to use PTHREAD_STACK_MIN

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Include the task headers */
#include "master_handler.h"
#include "types.h"
#include "logger.h"

/* Define the maximum number of messages and the maximum message size */
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE sizeof(QueueMessage)  

// Queue handle 
static QueueHandle_t xQueue; 

static RetVal initComponents() {
    xQueue = xQueueCreate(MAX_MESSAGES, MAX_MSG_SIZE);

    printf("xQueue Address: %p\n", (void *)xQueue);

    if (xQueue == NULL) {
        logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create queue\n");
        return RET_ERROR;
    } else {
        logMessage(LOG_LEVEL_INFO, "Main", "Queue created successfully\n");
    }

    if(initStateSemaphoreMaster() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init State Semaphore Master faild");
        return RET_ERROR;
    }

    return RET_OK;
} 

static RetVal creatMasterTasks(){
    // /* Create the master task */
    // if (xTaskCreate(vMasterCommHandler, "MasterTask", PTHREAD_STACK_MIN, NULL, 
    //                 TASTK_PRIO_MASTER_COMM_HANDLER, NULL) != pdPASS) {
    //     logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
    //     return RET_ERROR;
    // } else {
    //     logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    // }

    // /* Create the master task */
    // if (xTaskCreate(vMasterStatusCheckHandler, "MasterStatusCheckHandler", PTHREAD_STACK_MIN, NULL, 
    //                 TASTK_PRIO_MASTER_STATUS_CHECK_HANDLER, NULL) != pdPASS) {
    //     logMessage(LOG_LEVEL_ERROR, "Main", "Failed to create MasterTask\n");
    //     return RET_ERROR;
    // } else {
    //     logMessage(LOG_LEVEL_INFO, "Main", "MasterTask created successfully\n");
    // }

    xTaskCreate(vCreateTCPServer, "CreateTCPServer", configMINIMAL_STACK_SIZE * 4, NULL, 
                TASTK_PRIO_MASTER_TCP_SERVER_HANDLER, NULL);

    xTaskCreate(vCreateTCPClient, "CreateTCPClient", configMINIMAL_STACK_SIZE * 4, xQueue, 
                TASTK_PRIO_MASTER_TCP_CLIENT_HANDLER, NULL);

    // xTaskCreate(vRunStateMachine, "RunStateMachine", configMINIMAL_STACK_SIZE * 4, xQueue, 
    //             TASTK_PRIO_MASTER_STATE_MACHINE_HANDLER, NULL);

    return RET_OK;
}

int main(void)
{  
    if(initComponents() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Init Components faild");
        return 1;
    }

    if(creatMasterTasks() != RET_OK){
        logMessage(LOG_LEVEL_ERROR, "Main", "Create Master Tasks faild");
        return 1;
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();
    return 0; 
}
