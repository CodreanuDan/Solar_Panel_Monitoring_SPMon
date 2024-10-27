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
#define ADC_PORT_TEMP_SENS 3

#define ADC_MAX_VAL 4095u
#define ADC_MIN_VAL 0u

#define MAX_VAL_TEMP 100u
#define MIN_VAL_TEMP 0u

#define MAX_VAL_HUM 100u
#define MIN_VAL_HUM 0u

#define MAX_VAL_LUX 100u
#define MIN_VAL_LUX 0u

#define CONV_FACTOR 4095u
#define CONV_DIVISOR 12u

#define STACK_SIZE_BYTES 2048u
#define TRUE 1u
#define FALSE 0u

typedef void (*TASK_FUNC)();

typedef void (*FUNC_COM_TXRX)();
typedef bool (*FUNC_COM_CTRL)();
typedef uint8_t (*FUNC_COM_MON)();

typedef float(*FUNC_SEN_MEAS)();
typedef bool (*FUNC_SEN_CTRL)();
typedef uint8_t (*FUNC_SEN_MON)();

/* Sensor related errors*/
typedef enum{
  SEN_ERROR_UNDERVOLATGE = 0u,
  SEN_ERROR_OVERVOLTAGE,
  SEN_ERROR_NO_ADC_DATA
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
 uint8_t RawAdc_TempVal;
 uint8_t RawAdc_HumVal;
 uint8_t RawAdc_LuxVal;
}SenorRawValues;

/* Converted sensor values */
typedef struct{
  float ConValTemp;
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

/* Declare task priority */
static const uint8_t SPMon_InitTask_prio = 1;
static const uint8_t SPMon_SensCalibTask_prio = 2;
static const uint8_t SPMon_ComInitTask_prio = 3;
static const uint8_t SPMon_MainTask_prio = 1;

/* Declare sempahore handler */
extern SemaphoreHandle_t xInitSemaphore;


#endif