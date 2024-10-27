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

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/

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
    Serial.println(F("[MAIN_TASK_MAIN_FUNC/called_once_per_second]"));
    spmonMain.SPMon_MainTask_ExecuteStateLogic();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
 
}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateMainTask
* Descr: Function that creates the SPMon_MainTask
* Params:
*
******************************************************************************************************/
void SPMonMainTask::SPMon_MainTask_ExecuteStateLogic()
{
    Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/STARTED]"));
    vTaskDelay(250 / portTICK_PERIOD_MS);
    Serial.println(F("[SPMon_MainTask_ExecuteStateLogic/ENDED]"));
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
        Serial.println(F("[MAIN_TASK_CREATED]"));
    } 
    else 
    {
        Serial.println(F("[ERROR] Failed to create MAIN_TASK_"));
    }
}