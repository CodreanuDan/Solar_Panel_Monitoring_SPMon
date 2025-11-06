/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_MainTask.cpp
* Descr: Contains the functions for SPMon_MainTask
*
*
*
*
*
* Version: 1.0, 26.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_MainTask.h"
#include "SPMon_SenMeasTask.h"

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/
TaskHandle_t SPMon_SenMeasTask_hdl = NULL;

/******************************************************************************************************
* Function name: SPMon_MainTask_MainFunc
* Descr: Main function for the SPMon_MainTask
* Params: parameter(unused)
*
******************************************************************************************************/
void SPMon_MainTask_MainFunc(void *parameter)
{
  for(;;)
  {
    // Serial.println(F("[MAIN_TASK_MAIN_FUNC/called_once_per_second]"));
    /* Declare local variables */
    TaskStateMng taskState;
    /* Execute the state machine */
    spmonMain.SPMon_MainTask_ExecuteStateLogic(&initFlag, &taskState);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
 
}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateMainTask
* Descr: Function that creates the SPMon_MainTask
* Params:
*
******************************************************************************************************/
void SPMonMainTask::SPMon_MainTask_ExecuteStateLogic(InitFlags * InitFlag, TaskStateMng * TaskState)
{
    // Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/STARTED]"));
    if(InitFlag->COM_TASK_FLAG == FALSE && InitFlag->SEN_TASK_FLAG == FALSE)
    {
        // Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/COM_TASK_FLAG_FALSE&SEN_TASK_FLAG_FALSE]"));
        InitFlag->COM_TASK_FLAG = TRUE;
        InitFlag->SEN_TASK_FLAG = TRUE;
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    /* Here is the main logic of the measurement and data transmission process*/
    if (InitFlag->COM_TASK_FLAG == TRUE && InitFlag->SEN_TASK_FLAG == TRUE)
    {
        // Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/COM_TASK_FLAG_TRUE&SEN_TASK_FLAG_TRUE]:"));
        if(!sensor_measurement.SPMon_SenMeasTask_CreateSenMeasTaskTask())
        {
            TaskState->SenMeasTaskState = SENS_MEAS_STATE_OFF;
        }
        else
        {
            TaskState->SenMeasTaskState = SENS_MEAS_STATE_MEAS;
        }
    }
    else
    {
        InitFlag->COM_TASK_FLAG = FALSE;
        InitFlag->SEN_TASK_FLAG = FALSE;
    }

    vTaskDelay(MAIN_TASK_PERIOD / portTICK_PERIOD_MS);
    // Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/ENDED]"));

}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateMainTask
* Descr: Function that creates the SPMon_MainTask
* Params:
*
******************************************************************************************************/
void SPMonMainTask::SPMon_MainTask_CreateMainTask()
{
    
  /* Create task */
  if(xTaskCreatePinnedToCore(
        /* Task main function */
        SPMon_MainTask_MainFunc,  
        /* Task name */  
        "SPMon_MainTask", 
        /* Stack size (bytes) */  
        STACK_SIZE_BYTES, 
        /* Parameter */
        NULL,
        /* Task priority */            
        SPMon_MainTask_prio, 
        /* Task handler */  
        &SPMon_MainTask_hdl,  
        /* Task running on core (1 or 0) */  
        0                
    ) == pdPASS)
    {
        // Serial.println(F("[MAIN_TASK_CREATED]"));
    } 
    else 
    {
        // Serial.println(F("[ERROR] Failed to create MAIN_TASK_"));
    }
}