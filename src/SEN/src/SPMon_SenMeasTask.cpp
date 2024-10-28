/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_SenMeasTask.cpp
* Descr: Contains the functions for SPMon_SenMeasTask
*
*
*
*
*
* Version: 1.0, 28.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_SenMeasTask.h"

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/

/******************************************************************************************************
* Function name: SPMon_SenMeasTask_MainFunc
* Descr: Main function for the SPMon_SenMeasTask_MainFunc protoype
* Params: none
*
******************************************************************************************************/
void SPMon_SenMeasTask_MainFunc(void *parameter)
{
  for(;;)
  {
    Serial.println(F("[SEN_MEAS_TASK_MAIN_FUNC/called_once_per_second]"));
    vTaskDelay(SEN_MEAS_TASK_PERIOD / portTICK_PERIOD_MS);
  }
 
}

/******************************************************************************************************
* Function name: SPMon_SenMeasTask_CreateSenMeasTask
* Descr: Function that creates the SPMon_SenMeasTask
* Params:
*
******************************************************************************************************/
void SPMon_SensorMeasurementTask::SPMon_SenMeasTask_CreateSenMeasTaskTask()
{
      
  /* Create task */
  if(xTaskCreatePinnedToCore(
        /* Task main function */
        SPMon_SenMeasTask_MainFunc,  
        /* Task name */  
        "SPMon_SenMeasTask",
        /* Stack size (bytes) */  
        STACK_SIZE_BYTES, 
        /* Parameter */
        NULL,
        /* Task priority */            
        SPMon_SenMeasTask_prio, 
        /* Task handler */  
        &SPMon_SenMeasTask_hdl,  
        /* Task running on core (1 or 0) */  
        1              
    ) == pdPASS)
    {
        Serial.println(F("[SEN_MEAS_TASK_CREATED]"));
    } 
    else 
    {
        Serial.println(F("[ERROR] Failed to create SEN_MEAS_TASK"));
    }
}

