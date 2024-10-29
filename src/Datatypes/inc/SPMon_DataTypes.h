/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_DataTypes.h
* Descr: Contains the datatypes and variables for the project
*
*
*
*
*
* Version: 1.0, 19.10.2024
******************************************************************************************************/
#ifndef SPMON_DATATYPES_H
#define SPMON_DATATYPES_H

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include <cstdint>
#include <cstddef>
#include <Arduino.h>

/******************************************************************************************************
* Datatypes 
******************************************************************************************************/
#define ADC_PORT_LIGHT_SENS 1
#define ADC_PORT_HUM_SENS 2
#define ADC_PORT_LM_35 4

#define ADC_MAX_VAL 4095u
#define ADC_MIN_VAL 0u
#define ADC_RESOLUTION 12u

#define POWER_SUPPLY_VOLTAGE_5V 5000.00
#define POWER_SUPPLY_VOLTAGE_3V3 3300.00

#define MAX_VAL_TEMP 100u
#define MIN_VAL_TEMP 0u

#define MAX_VAL_HUM 100u
#define MIN_VAL_HUM 0u

#define MAX_VAL_LUX 100u
#define MIN_VAL_LUX 0u

#define CONV_FACTOR 4095u
#define CONV_DIVISOR 10u

#define STACK_SIZE_BYTES 2048u
#define TRUE 1u
#define FALSE 0u

#define LM35_CALIBRATION_OFFSET 8.5f
#define LM35_MAX_SAMPLES 10u 
#define LM35_CALIBRATION_PERIOD 1000u
#define LM35_CALIBRATION_SAMPLES 10u
#define LM35_ERROR_THRESHOLD 5.5f

#define FAULT_SENS_COUNTER 3u

#define SEN_MEAS_TASK_PERIOD 1000u
#define COM_TASK_PERIOD 1000u
#define MAIN_TASK_PERIOD 1000u

/* Sensor related errors*/
typedef enum{
  SEN_ERROR_NO_ERROR = 0u,
  SEN_ERROR_UNDERVOLATGE,
  SEN_ERROR_OVERVOLTAGE,
  SEN_ERROR_NO_RESPONSE,
  SEN_ERROR_NO_CALIBRATION,
  SEN_ERROR_CALIBRATION_ERROR,
  SEN_ERROR_NO_MEASUREMENT,
  SEN_ERROR_MEASUREMENT_ERROR,
  SEN_ERROR_ADC_ERROR
}SensorElectricalError;

/* Sensor list*/
typedef enum{
  SENS_TEMP_DHT_11 = 0u,
  SENS_TEMP_LM_35,
  SENS_TEMP_THCPL,
  SENS_LIGHT_BH_1750,
  SENS_DUST,
  SENS_CURR_1,
  SENS_CURR_2
}SensorList;

/* Sensor task states*/
typedef enum{
  SENS_MEAS_STATE_OFF = 0u,
  SENS_MEAS_STATE_MEAS,
  SENS_MEAS_STATE_CONV,
  SENS_MEAS_STATE_ERROR
}SenMeasStates;

/* Init Task Flags*/
typedef struct{
bool SEN_CALIB_FLAG;
bool COM_INIT_FLAG;
bool COM_TASK_FLAG;
bool SEN_TASK_FLAG;
}InitFlags;
extern InitFlags initFlag; 

/* Raw sensor values */
typedef struct{
 uint16_t RawAdc_TempVal_LM35;
 uint8_t RawAdc_HumVal;
 uint8_t RawAdc_LuxVal;
}SenorRawValues;

/* Converted sensor values */
typedef struct{
  float ConValTempLM35;
  float ConValHum;
  float ConValLux;
}SenorConvertedValues;

/* Sensor data */
typedef struct{
  SensorList SensorType;
  SensorElectricalError ErrorType;
  bool PlausResult;
  uint8_t FaultSensCouter;
}SensorErrorMonitoring;

/*Task State (might be removed later)*/
typedef struct{
  uint8_t InitTaskState;
  uint8_t SensCalibState;
  uint8_t ComInitTaskState;
}TaskStateMng;

/* InitTaskSemaph */
typedef struct {
    TaskStateMng *taskState;
    SemaphoreHandle_t xSemaphore;
}SPMon_TaskInitParams;

/* Declare task handlers */
extern TaskHandle_t SPMon_InitTask_hdl;
extern TaskHandle_t SPMon_SensCalibTask_hdl;
extern TaskHandle_t SPMon_ComInitTask_hdl;
extern TaskHandle_t SPMon_MainTask_hdl;
extern TaskHandle_t SPMon_SenMeasTask_hdl;
extern TaskHandle_t SPMon_ComTask_hdl;

/* Declare task priority */
static const uint8_t SPMon_InitTask_prio = 1;
static const uint8_t SPMon_SensCalibTask_prio = 2;
static const uint8_t SPMon_ComInitTask_prio = 3;
static const uint8_t SPMon_MainTask_prio = 1;
static const uint8_t SPMon_SenMeasTask_prio = 2;
static const uint8_t SPMon_ComTask_prio = 3;

/* Declare sempahore handler */
extern SemaphoreHandle_t xInitSemaphore;


#endif