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
#include "SPMon_lm35_lib.h"
#include "SPMon_MxThcpl_lib.h"
#include "SPMon_dht11_lib.h"
#include <vector>

/******************************************************************************************************
 * FILE LOCAL VARIABLES
 ******************************************************************************************************/
/* Object for thermocouple sensor */
SPMon_MAX6675_THCPL_Sensor_Library thermocouple(SPI_PIN_SCLK, SPI_PIN_CS, SPI_PIN_MISO);
/* Object for DHT11 sensor */
SPMon_DHT11_Sensor_Library dht11(DHT11_PIN);
/* Task flag */
bool SPMon_SensorMeasurementTask::taskCreated = FALSE;

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_MainFunc
 * Descr: Main function for the SPMon_SenMeasTask_MainFunc protoype
 * Params: none
 *
 ******************************************************************************************************/
void SPMon_SenMeasTask_MainFunc(void *parameter)
{
  for (;;)
  {
    Serial.println(F("[SEN_MEAS_TASK_MAIN_FUNC/called_once_per_second]"));
    /* Execute the state machine */
    sensor_measurement.SPMon_SenMeasTask_ExecuteStateMachine(&TaskState, &RawValues, &ConvertedValues);
    vTaskDelay(SEN_MEAS_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ExecuteStateMachine
 * Descr: Function that executes the state machine for the sensor measurement task
 * Params: TaskStateMng *taskState - pointer to the task state management structure
 * Returns: none
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask::SPMon_SenMeasTask_ExecuteStateMachine(TaskStateMng *taskState, SensorRawValues *rawValues, SensorConvertedValues *convertedValues)
{
  Serial.println(F("[SPMon_SenMeasTask_ExecuteStateMachine]"));
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  /* Create a vector of sensor measurement interfaces */
  std::vector<SPMon_SensorMeasurementTask_Interface *> sensorMeasurementInterface;
  /* Add the sensor measurement interfaces to the vector */

/* Check LM35 sensor enable flag */
#if LM35_SENSOR_ENABLE == TRUE
  sensorMeasurementInterface.push_back(new SPMon_SensorMeasurementTask_LM35());
#endif
/* Check MAXTCHPL sensor enable flag */
#if THCPL_SENSOR_ENABLE == TRUE
  sensorMeasurementInterface.push_back(new SPMon_SensorMeasurementTask_MAXTCHPL());
#endif
/* Check DHT11 sensor enable flag */
#if DHT11_SENSOR_ENABLE == TRUE
  sensorMeasurementInterface.push_back(new SPMon_SensorMeasurementTask_DHT11());
#endif

  /* Execute the state machine */
  switch (taskState->SenMeasTaskState)
  {
  /* Turn on the sensors if no errors are detected */
  case SENS_MEAS_STATE_OFF:
    Serial.println(F("[STATE] SENS_MEAS_STATE_OFF"));
    taskState->SenMeasTaskState = SENS_MEAS_STATE_MEAS; 
    break;
  /* Get raw data from the sensors */
  case SENS_MEAS_STATE_MEAS:
    Serial.println(F("[STATE] SENS_MEAS_STATE_MEAS"));
    /* Get raw data from the sensors */
    for (SPMon_SensorMeasurementTask_Interface *object : sensorMeasurementInterface)
    {
      if (object != nullptr)
      {
        /* Call the function */
        object->SPMon_SenMeasTask_GetRawData(rawValues);
        /* Delete the object */
        delete object;
      }
      else
      {
        Serial.println(F("[ERROR] Null pointer detected in SENS_MEAS_STATE_MEAS"));
      }
    }
    taskState->SenMeasTaskState = SENS_MEAS_STATE_CONV; 
    break;
  /* Convert raw data from the sensors */
  case SENS_MEAS_STATE_CONV:
    Serial.println(F("[STATE] SENS_MEAS_STATE_CONV"));
    /* Convert raw data from the sensors */
    for (SPMon_SensorMeasurementTask_Interface *object : sensorMeasurementInterface)
    {
      if (object != nullptr)
      {
        /* Call the function */
        object->SPMon_SenMeasTask_ConvertData(rawValues, convertedValues);
        /* Delete the object */
        delete object;
      }
      else
      {
        Serial.println(F("[ERROR] Null pointer detected in SENS_MEAS_STATE_CONV"));
      }
    }
    taskState->SenMeasTaskState = SENS_MEAS_STATE_MEAS; 
    break;
  /* Error state error dissapears -> SENS_MEAS_STATE_OFF else SENS_MEAS_STATE_OFF (to be implemented) */
  case SENS_MEAS_STATE_ERROR:
    Serial.println(F("[STATE] SENS_MEAS_STATE_ERROR"));
    break;
  default:
    Serial.println(F("[ERROR] Unknown state"));
    break;
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_CreateSenMeasTask
 * Descr: Function that creates the SPMon_SenMeasTask
 * Params:
 * Rs: bool - TRUE if the task is created, FALSE if the task is already created
 *
 ******************************************************************************************************/
bool SPMon_SensorMeasurementTask::SPMon_SenMeasTask_CreateSenMeasTaskTask()
{

  /* Check if the task is created */
  if (!taskCreated)
  {

    /* Create task */
    if (xTaskCreatePinnedToCore(
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
            1) == pdPASS)
    {
      Serial.println(F("[SEN_MEAS_TASK_CREATED]"));
    }
    else
    {
      Serial.println(F("[ERROR] Failed to create SEN_MEAS_TASK"));
    }
    taskCreated = TRUE;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/******************************************************************************************************
 * ----------------------------------------------------------------------------------------------------
 * SPMon_SensorMeasurementTask_Interface CLASS FUNCTIONS
 * @descr: FUNCTION IMPLEMENTATIONS FOR THE INTERFACE CLASS SPMon_SensorMeasurementTask_Interface in
 * the interface pointer vector in the SPMon_SenMeasTask_ExecuteStateMachine functions
 * ----------------------------------------------------------------------------------------------------
 * ****************************************************************************************************/


/******************************************************************************************************
 * LM 35 SENSOR FUNCTIONS
 ******************************************************************************************************/

/******************************************************************************************************
 * Constructor
 *******************************************************************************************************/
SPMon_SensorMeasurementTask_LM35::SPMon_SensorMeasurementTask_LM35()
{
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_GetRawData(LM35)
 * Descr: Function that gets the raw data from the LM35 sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_LM35::SPMon_SenMeasTask_GetRawData(SensorRawValues *rawValues)
{
  if (rawValues != nullptr)
  {
    /* Get raw data from the LM35 sensor */
    lm35.LM35_GetRawData(rawValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_GetRawData[LM35]"));
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ConvertData(LM35)
 * Descr: Function that converts the raw data from the LM35 sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *         SensorConvertedValues * convertedValues - pointer to the converted values structure
 *         SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 ******************************************************************************************************/
void SPMon_SensorMeasurementTask_LM35::SPMon_SenMeasTask_ConvertData(SensorRawValues *rawValues, SensorConvertedValues *convertedValues)
{
  if (rawValues != nullptr && convertedValues != nullptr)
  {
    /* Convert raw data from the LM35 sensor */
    lm35.LM35_GetTemp(rawValues, convertedValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_ConvertData[LM35]"));
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ErrorMonitor(LM35)
 * Descr: Function that monitors the errors from the LM35 sensor, overrides the virtual function from the
 *       SPMon_SensorMeasurementTask_Interface class
 * Params: SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_LM35::SPMon_SenMeasTask_ErrorMonitor()
{
}

/******************************************************************************************************
 * THCPLE SENSOR FUNCTIONS
 ******************************************************************************************************/

/******************************************************************************************************
 * Constructor
 *******************************************************************************************************/
SPMon_SensorMeasurementTask_MAXTCHPL::SPMon_SensorMeasurementTask_MAXTCHPL()
{
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_GetRawData(THCPLE)
 * Descr: Function that gets the raw data from the THCPLE sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_MAXTCHPL::SPMon_SenMeasTask_GetRawData(SensorRawValues *rawValues)
{
  if (rawValues != nullptr)
  {
    /* Get raw data from the THCPLE sensor */
    thermocouple.MAX6675_GetRawData(rawValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_GetRawData[THCPLE]"));
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ConvertData(THCPLE)
 * Descr: Function that converts the raw data from the THCPLE sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *         SensorConvertedValues * convertedValues - pointer to the converted values structure
 *         SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 ******************************************************************************************************/
void SPMon_SensorMeasurementTask_MAXTCHPL::SPMon_SenMeasTask_ConvertData(SensorRawValues *rawValues, SensorConvertedValues *convertedValues)
{
  if (rawValues != nullptr && convertedValues != nullptr)
  {
    /* Convert raw data from the THCPLE sensor */
    thermocouple.MAX6675_GetTemp(rawValues, convertedValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_ConvertData[THCPLE]"));
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ErrorMonitor(THCPLE)
 * Descr: Function that monitors the errors from the THCPLE sensor, overrides the virtual function from the
 *       SPMon_SensorMeasurementTask_Interface class
 * Params: SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_MAXTCHPL::SPMon_SenMeasTask_ErrorMonitor()
{
}

/******************************************************************************************************
 * DHT11 SENSOR FUNCTIONS
 ******************************************************************************************************/

/******************************************************************************************************
 * Constructor
 ******************************************************************************************************/
SPMon_SensorMeasurementTask_DHT11::SPMon_SensorMeasurementTask_DHT11()
{
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_GetRawData(DHT11)
 * Descr: Function that gets the raw data from the DHT11 sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_DHT11::SPMon_SenMeasTask_GetRawData(SensorRawValues *rawValues)
{
  if (rawValues != nullptr)
  {
    /* Get raw data from the DHT11 sensor */
    dht11.DHT11_GetRawData(rawValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_GetRawData[DHT11]"));
  }
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ConvertData(DHT11)
 * Descr: Function that converts the raw data from the DHT11 sensor, overrides the virtual function from the
 *        SPMon_SensorMeasurementTask_Interface class
 * Params: SensorRawValues * rawValues - pointer to the raw values structure
 *         SensorConvertedValues * convertedValues - pointer to the converted values structure
 *         SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 *******************************************************************************************************/
void SPMon_SensorMeasurementTask_DHT11::SPMon_SenMeasTask_ConvertData(SensorRawValues *rawValues, SensorConvertedValues *convertedValues)
{
  if (rawValues != nullptr && convertedValues != nullptr)
  {
    /* Convert raw data from the DHT11 sensor */
    dht11.DHT11_GetConvData(rawValues, convertedValues);
  }
  else
  {
    Serial.println(F("[ERROR] Null pointer passed to SPMon_SenMeasTask_ConvertData[DHT11]"));
  }
  
}

/******************************************************************************************************
 * Function name: SPMon_SenMeasTask_ErrorMonitor(DHT11)
 * Descr: Function that monitors the errors from the DHT11 sensor, overrides the virtual function from the
 *      SPMon_SensorMeasurementTask_Interface class
 * Params: SensorErrorMonitoring * sensorError - pointer to the sensor error structure
 *
 * *******************************************************************************************************/
void SPMon_SensorMeasurementTask_DHT11::SPMon_SenMeasTask_ErrorMonitor()
{
}
