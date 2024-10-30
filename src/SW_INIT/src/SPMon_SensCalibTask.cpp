#include "freertos/projdefs.h"
/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_SensCalibTask.h.cpp
* Descr: Contains the functions for SPMon_SensCalibTask
*
*
*
*
*
* Version: 1.1 , 30.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_SensCalibTask.h"
#include "SPMon_lm35_lib.h"

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/

/******************************************************************************************************
* Function name: SPMon_SensCalibTask_MainFunc
* Descr: Main function for the SPMon_SensCalibTask_MainFunc
* Params: parameter(unused)
*
******************************************************************************************************/
void SPMon_SensCalibTask_MainFunc(void *parameter)
{

  // Serial.println(F("[SENS_CALIB_TASK_MAIN_FUNC_STARTED]"));

  SPMon_TaskInitParams *taskParamsPtr = (SPMon_TaskInitParams*)parameter;
  TaskStateMng *taskStatePtr = taskParamsPtr->taskState;
  SemaphoreHandle_t xSemaphore = taskParamsPtr->xSemaphore;


  if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) 
  {
    // Serial.println(F("[SENS_CALIB_TASK_TOOK_SEMAPHORE]"));
    delay(250);
    taskStatePtr->SensCalibState = eTaskGetState(SPMon_SensCalibTask_hdl);

    if(xSemaphoreGive(xSemaphore))
    {
      vTaskDelay(100 / portTICK_PERIOD_MS);
      /* Call the function and begin sensor calibration */
      senCal.SPMon_SensCalibTask_SensCalibFunction(&initFlag);
      // Serial.println(F("[SENS_CALIB_TASK_COMPLETED]"));
      vTaskSuspend(NULL);
      taskStatePtr->SensCalibState = eTaskGetState(SPMon_SensCalibTask_hdl);
    }

  }
  else 
  {
    // Serial.println(F("[ERROR] Failed to take semaphore in SensCalibTask"));
  }

}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateSensCalibTask
* Descr: Function that creates the SensCalibTask
* Params:
*
******************************************************************************************************/
void SPMonSensCalibTask::SPMon_SensCalibTask_CreateSensCalibTask()
{
  /* Create new instance for TaskStateMng struct to be passed to the task param*/
  TaskStateMng *taskState = new TaskStateMng();

  /* Create new strcture for params */
  SPMon_TaskInitParams *taskParamsPtr = new SPMon_TaskInitParams();
  taskParamsPtr->taskState = taskState;
  taskParamsPtr->xSemaphore = xInitSemaphore;

  /* Create task */
  if(xTaskCreatePinnedToCore(
        /* Task main function */
        SPMon_SensCalibTask_MainFunc,  
        /* Task name */  
        "SPMon_SensCalibTask", 
        /* Stack size (bytes) */  
        STACK_SIZE_BYTES, 
        /* Parameter */
        (void*)taskParamsPtr, 
        /* Task priority */            
        SPMon_SensCalibTask_prio, 
        /* Task handler */  
        &SPMon_SensCalibTask_hdl,  
        /* Task running on core (1 or 0) */  
        1              
    ) == pdPASS)
    {
        // Serial.println(F("[SENS_CALIB_TASK_CREATED]"));
    } 
    else 
    {
        // Serial.println(F("[ERROR] Failed to create SENS_CALIB_TASK"));
    }
}

/******************************************************************************************************
* Function name: SPMon_SensCalibTask_SensCalibFunction
* Descr: Function that manages the sensor calibration process
* Params:
*
******************************************************************************************************/
void SPMonSensCalibTask::SPMon_SensCalibTask_SensCalibFunction(InitFlags * CalibFlag)
{
  // Serial.println(F("[SPMon_SensCalibTask_SensCalibFunction/STARTED]"));
  CalibFlag->SEN_CALIB_FLAG = TRUE;
  // Serial.println(F("[SEN_CALIB_FLAG_SET]"));
  senCal.SPMon_SensCalibTask_CalibrateSensors();
  senCal.SPMon_SensCalibTask_CheckCalibration();
  vTaskDelay(250 / portTICK_PERIOD_MS);
  // Serial.println(F("[SPMon_SensCalibTask_SensCalibFunction/ENDED]"));
}

/******************************************************************************************************
* Function name: SPMon_SensCalibTask_CalibrateSensors
* Descr: Function that manages the sensor calibration for each sensor
* Params:
*
******************************************************************************************************/
void SPMonSensCalibTask::SPMon_SensCalibTask_CalibrateSensors()
{
  // Serial.println(F("[SPMon_SensCalibTask_CalibrateSensors/STARTED]"));
  lm35.LM35_Calib(ADC_PORT_LM_35, 12, ADC_6db);
  vTaskDelay(250 / portTICK_PERIOD_MS);
  // Serial.println(F("[SPMon_SensCalibTask_CalibrateSensors/ENDED]"));
}

/******************************************************************************************************
* Function name: SPMon_SensCalibTask_CheckCalibration
* Descr: Function that checks if the calibration it`s done for each sensor
* Params:
*
******************************************************************************************************/
void SPMonSensCalibTask::SPMon_SensCalibTask_CheckCalibration()
{
  // Serial.println(F("[SPMon_SensCalibTask_CheckCalibration/STARTED]"));
  vTaskDelay(250 / portTICK_PERIOD_MS);
  // Serial.println(F("[SPMon_SensCalibTask_CheckCalibration/ENDED]"));
}