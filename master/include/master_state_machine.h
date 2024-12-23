#ifndef MASTER_STATE_MACHINE_H
#define MASTER_STATE_MACHINE_H

#include "types.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * @brief Initializes the state semaphore for master state synchronization.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal initStateSemaphoreMaster();

/**
 * @brief Dispatches states to appropriate state handlers.
 * @param data The message received.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
// void stateDispatcher(void* args);
RetVal stateDispatcher(MasterStates state);

/**
 * @brief Retrieves the current state of the master.
 * @return The current state of the master.
 */
MasterStates getCurrentState();

/**
 * @brief Sets the state queue handler.
 * @param stateQueueHandle The queue handle to set.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal setStateQueueHandlerMaster(QueueHandle_t stateQueueHandle);

/**
 * @brief Retrieves the state queue handler.
 * @param stateQueueHandle The queue handle to retrieve.
 * @return RET_OK if successful, RET_ERROR otherwise.
 */
RetVal getStateQueueHandlerMaster(QueueHandle_t stateQueueHandle);

#endif // MASTER_STATE_MACHINE_H