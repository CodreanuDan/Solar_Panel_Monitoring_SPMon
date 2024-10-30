/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_InitTask.cpp
* Descr: Contains the functions for SPMon_InitTask
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
#include "SPMon_InitTask.h"
#include "SPMon_SensCalibTask.h"
#include "SPMon_ComInitTask.h"
#include "SPMon_MainTask.h"

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/
/*  Task handlers definition*/
TaskHandle_t SPMon_InitTask_hdl = NULL;
TaskHandle_t SPMon_ComInitTask_hdl = NULL;
TaskHandle_t SPMon_SensCalibTask_hdl = NULL;
TaskHandle_t SPMon_MainTask_hdl = NULL;
/* Semaphore handler defintion */
SemaphoreHandle_t xInitSemaphore = NULL;
/* Definition for initflags */
InitFlags initFlag = {FALSE, FALSE, FALSE, FALSE};

/******************************************************************************************************
* Function name: SPMon_InitTask_MainFunc
* Descr: Main function for the SPMon_InitTask
* Params: parameter: pointer to TaskStateMng struct
*
******************************************************************************************************/
void SPMon_InitTask_MainFunc(void *parameter) 
{
  // Serial.println(F("[INIT_TASK_MAIN_FUNC_STARTED]"));

  bool local_val_suspendTask = FALSE;
  
  bool INIT_RES = spmonInit.SPMon_InitTask_CheckInitData(parameter);
  if (INIT_RES == TRUE) 
  {

    // Serial.println(F("[AFTER_CHECK_INIT_DATA]"));

    /* Create semaphore to trigger SENS/COM INIT Tasks */
    xInitSemaphore = xSemaphoreCreateBinary();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    /* Check the creation of the sempahore */
    if (xInitSemaphore != NULL) 
    {
      // Serial.println(F("[INIT_SEMAPHORE_CREATED]"));

      /* Create the tasks for SenCalib and ComInit*/
      senCal.SPMon_SensCalibTask_CreateSensCalibTask();
      comInit.SPMon_ComInitTask_CreateComInitTask();
  
      /* Check if the tasks are created */
      InitFlags * initFlagPtr = &initFlag;

      /* Free the semphore to be taken by the other tasks */
      xSemaphoreGive(xInitSemaphore);

      /* Wait for the tasks to be completed */
      while(!initFlagPtr->SEN_CALIB_FLAG || !initFlagPtr->COM_INIT_FLAG)
      {
        vTaskDelay(100 / portTICK_PERIOD_MS);
      }

      /* Check if the tasks are completed, we can suspend the init task*/
      if(initFlagPtr->SEN_CALIB_FLAG == TRUE && initFlagPtr->COM_INIT_FLAG == TRUE)
      {
        // Serial.println(F("[SENS_COM_INIT_&_COM_INIT_COMPLETED]"));
        spmonMain.SPMon_MainTask_CreateMainTask();
        local_val_suspendTask = TRUE;
      }

    } 
    else 
    {
      return;
    }
  } 
  else 
  {

  }

  if(local_val_suspendTask == TRUE)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    // Serial.println(F("[INIT_TASK_COMPLETED]"));
    vTaskSuspend(NULL);
  }

}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateInitTask
* Descr: Function that check the state of the init task
* Params: void* parameter from main function
*
******************************************************************************************************/
bool SPMonInitTask::SPMon_InitTask_CheckInitData(void *parameter) 
{
  /* Convert the paramater to TaskStateMng type */
  TaskStateMng *taskState = (TaskStateMng *)parameter;

  /* Initialize data  in oder to avoid garbage values or null pointers */
  // taskState->InitTaskState = 0;
  // taskState->SensCalibState = 0;
  // taskState->ComInitTaskState = 0;
  // taskState->SenMeasTaskState = SENS_MEAS_STATE_OFF;

  // initFlag.SEN_CALIB_FLAG = FALSE;
  // initFlag.COM_INIT_FLAG = FALSE;
  // initFlag.COM_TASK_FLAG = FALSE;
  // initFlag.SEN_TASK_FLAG = FALSE;

  // SensorRawValues rawValues = {0};
  // SensorConvertedValues convertedValues = {0};
  // SensorErrorMonitoring sensorError = {SENS_TEMP_LM_35, SEN_ERROR_NO_ERROR, false, 0};

  bool init_res = false;
  if (eTaskGetState(SPMon_InitTask_hdl) == 0) 
  {
    if (initFlag.SEN_CALIB_FLAG == FALSE && initFlag.COM_INIT_FLAG == FALSE) 
    {
      taskState->InitTaskState = eTaskGetState(SPMon_InitTask_hdl);
      init_res = true;
    } 
    else 
    {
      taskState->InitTaskState = eTaskGetState(SPMon_InitTask_hdl);
      init_res = false;
    }
  } 
  else 
  {
    taskState->InitTaskState = eTaskGetState(SPMon_InitTask_hdl);
    ESP.restart();
  }

  return init_res;

}

/******************************************************************************************************
* Function name: SPMon_InitTask_CreateInitTask
* Descr: Function that creates the init task
* Params:
*
******************************************************************************************************/
void SPMonInitTask::SPMon_InitTask_CreateInitTask() 
{
  /* Create new instance for TaskStateMng struct to be passed to the task param*/
  TaskStateMng *taskState = new TaskStateMng();

  /* Create task */
  if (xTaskCreatePinnedToCore(
        /* Task main function */
        SPMon_InitTask_MainFunc,
        /* Task name */
        "SPMon_InitTask",
        /* Stack size (bytes) */
        STACK_SIZE_BYTES,
        /* Parameter */
        (void *)taskState,
        /* Task priority */
        SPMon_InitTask_prio,
        /* Task handler */
        &SPMon_InitTask_hdl,
        /* Task running on core (1 or 0) */
        0)
      == pdPASS) 
  {
    // Serial.println(F("[INIT_TASK_CREATED]"));
  } 
  else 
  {
    // Serial.println(F("[ERROR] Failed to create INIT_TASK"));

  }
}
