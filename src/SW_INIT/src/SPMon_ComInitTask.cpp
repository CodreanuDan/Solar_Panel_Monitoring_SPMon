/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_ComInitTask.cpp
* Descr: Contains the functions for SPMon_ComInitTask
*
*
*
*
*
* Version: 1.0, 20.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_ComInitTask.h"

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/


/******************************************************************************************************
* Function name: SPMon_ComInitTask_MainFunc
* Descr: Main function for the SPMon_ComInitTask
* Params: parameter(unused)
*
******************************************************************************************************/
void SPMon_ComInitTask_MainFunc(void *parameter)
{
  Serial.println(F("[COM_INIT_TASK_MAIN_FUNC_STARTED]"));
  SPMon_TaskInitParams *taskParams = (SPMon_TaskInitParams*)parameter;
  TaskStateMng *taskState = taskParams->taskState;
  SemaphoreHandle_t xSemaphore = taskParams->xSemaphore;

  if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) 
  {
      Serial.println(F("[COM_INIT_TASK_TOOK_SEMAPHORE]"));
      vTaskDelay(250 / portTICK_PERIOD_MS);
      taskState->ComInitTaskState = eTaskGetState(SPMon_ComInitTask_hdl);

      if(xSemaphoreGive(xSemaphore))
      {
        vTaskDelay(250 / portTICK_PERIOD_MS);

        /* Call the init function and begin wifi connection */
        comInit.SPMon_ComInitTaks_ComInitFunction(&initFlag);

        Serial.println(F("[COM_INIT_TASK_COMPLETED]"));
        vTaskSuspend(NULL);
        taskState->ComInitTaskState = eTaskGetState(SPMon_ComInitTask_hdl);
      }

  }
  else 
  {
      Serial.println(F("[ERROR] Failed to take semaphore in ComInitTask"));
  }

}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateComInitTask
* Descr: Function that creates the SensCalibTask
* Params:
*
******************************************************************************************************/
void SPMonComInitTask::SPMon_ComInitTask_CreateComInitTask()
{
  /* Create new instance for TaskStateMng struct to be passed to the task param*/
  TaskStateMng *taskState = new TaskStateMng();

  /* Create new strcture for params */
  SPMon_TaskInitParams *taskParams = new SPMon_TaskInitParams();
  taskParams->taskState = taskState;
  taskParams->xSemaphore = xInitSemaphore;


  /* Create task */
  if(xTaskCreatePinnedToCore(
        /* Task main function */
        SPMon_ComInitTask_MainFunc,  
        /* Task name */  
        "SPMon_ComInitTask", 
        /* Stack size (bytes) */  
        STACK_SIZE_BYTES, 
        /* Parameter */
        (void*)taskParams, 
        /* Task priority */            
        SPMon_ComInitTask_prio, 
        /* Task handler */  
        &SPMon_ComInitTask_hdl,  
        /* Task running on core (1 or 0) */  
        0                
    ) == pdPASS)
    {
        Serial.println(F("[COM_INIT_TASK_CREATED]"));
    } 
    else 
    {
        Serial.println(F("[ERROR] Failed to create COM_INIT_TASK"));
    }
}

/******************************************************************************************************
* Function name: SPMon_ComInitTaks_ComInitFunction
* Descr: Function that manages the wifi communication setup process
* Params:
*
******************************************************************************************************/
void SPMonComInitTask::SPMon_ComInitTaks_ComInitFunction(InitFlags * InitFlag)
{
  Serial.println(F("[SPMon_ComInitTaks_ComInitFunction/STARTED]"));
  InitFlag->COM_INIT_FLAG = TRUE;
  Serial.println(F("[COM_INIT_FLAG_SET]"));
  comInit.SPMon_ComInitTaks_EstablishWiFiComm();
  comInit.SPMon_ComInitTaks_CheckComInit();
  vTaskDelay(250 / portTICK_PERIOD_MS);
  Serial.println(F("[SPMon_ComInitTaks_ComInitFunction/ENDED]"));
}

/******************************************************************************************************
* Function name: SPMon_ComInitTaks_EstablishWiFiComm
* Descr: Function that manages the wifi communication init 
* Params:
*
******************************************************************************************************/
void SPMonComInitTask::SPMon_ComInitTaks_EstablishWiFiComm()
{
  Serial.println(F("[SPMon_ComInitTaks_EstablishWiFiComm/STARTED]"));
  vTaskDelay(250 / portTICK_PERIOD_MS);
  Serial.println(F("[SPMon_ComInitTaks_EstablishWiFiComm/ENDED]"));
}


/******************************************************************************************************
* Function name: SPMon_ComInitTaks_CheckComInit
* Descr: Function that checks if the communication is established
* Params:
*
******************************************************************************************************/
void SPMonComInitTask::SPMon_ComInitTaks_CheckComInit()
{
  Serial.println(F("[SPMon_ComInitTaks_CheckComInit/STARTED]"));
  vTaskDelay(250 / portTICK_PERIOD_MS);
  Serial.println(F("[SPMon_ComInitTaks_CheckComInit/ENDED]"));
}
