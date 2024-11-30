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

#define ADC_MAX_VAL 4095u
#define ADC_MIN_VAL 0u
#define ADC_RESOLUTION 12u

#define POWER_SUPPLY_VOLTAGE_5V 5000.00
#define POWER_SUPPLY_VOLTAGE_3V3 3300.00

#define MAX_VAL_TEMP 175u
#define MIN_VAL_TEMP -75

#define MAX_VAL_HUM 100u
#define MIN_VAL_HUM 0u

#define MAX_VAL_LUX 100u
#define MIN_VAL_LUX 0u

#define CONV_FACTOR 4095u
#define CONV_DIVISOR 10u

/*-----------------------------------------------*/
/* Code control defines */
#define STACK_SIZE_BYTES 2048u
#define TRUE 1u
#define FALSE 0u
/*-----------------------------------------------*/

/*------------------------------------------------*/
/* DEBUG OPTIONS 
   Debugging TRUE(1)-Enebled; FALSE(0)-Disabled */
#define DBG TRUE
/*------------------------------------------------*/


/*------------------------------------------------*/
/* LM35 Sensor related defines */

/* LM35 Sensor enable flag TRUE(1)-Enebled; FALSE(0)-Disabled */
#define LM35_SENSOR_ENABLE TRUE
/* Port for LM35 sensor */
#define ADC_PORT_LM_35 4
/* LM35 sensor calibration offset */
#define LM35_CALIBRATION_OFFSET 4.25f
/* LM 35 Oversampling enable flag TRUE(1)-Enebled; FALSE(0)-Disabled */
#define LM35_OVERSAMPLING TRUE
/* LM35 sensor oversampling rate */
#define LM35_MAX_SAMPLES 10u 
/* LM35 sensor calibration period */
#define LM35_CALIBRATION_PERIOD 1000u
/* LM35 sensor calibration samples */
#define LM35_CALIBRATION_SAMPLES 10u
/* LM35 sensor error threshold */
#define LM35_ERROR_THRESHOLD 5.5f
/*------------------------------------------------*/

/*------------------------------------------------*/
/* SPI related defines */

/* SPI pin: Serial Clock */
#define SPI_PIN_SCLK   18
/* SPI pin: Chip Select */
#define SPI_PIN_CS     5
/* SPI pin: Master In Slave Out */
#define SPI_PIN_MISO   19
/* SPI delay between bit read*/
#define SPI_DELAY_US 50u
/*------------------------------------------------*/

/*------------------------------------------------*/
/* MAXTCHPL Sensor related defines */

/* MAXTCHPL Sensor enable flag TRUE(1)-Enebled; FALSE(0)-Disabled */
#define THCPL_SENSOR_ENABLE TRUE
/* Thermocouple OverSampling enable flag TRUE(1)-Enebled; FALSE(0)-Disabled */
#define THCPL_OVERSAMPLING TRUE
/* Thermocouple related defines */
#define THERMOCOUPLE_OVERSAMPLING_RATE 10u
/* Conversion factor to temperature for thermocouple*/
#define THERMOCOUPLE_CONVERSION_FACTOR 0.25f
/*------------------------------------------------*/

/*------------------------------------------------*/
/* DHT11 Sensor related defines */

/* DHT11 Sensor enable flag TRUE(1)-Enebled; FALSE(0)-Disabled */
#define DHT11_SENSOR_ENABLE TRUE
/* DHT11 Sensor pin */
#define DHT11_PIN 13
/* DHT11 Sensor calibration period */
#define DHT11_CALIBRATION_PERIOD 500u
/*------------------------------------------------*/



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
extern SensorElectricalError SensorError;

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
 byte thCplRawData;
 /* DHT11 sensor raw data: 5 bytes */
 byte dhtRawData[5];
 uint16_t RawAdc_HumVal;
 uint16_t RawAdc_LuxVal;
}SensorRawValues;
extern SensorRawValues RawValues;

/* Converted sensor values */
typedef struct{
  float_t ConValTempLM35;
  float_t thCplConvData;
  /* DHT11 sensor values element: 0 - Humidity; 1 - Temperature */
  float_t ConValDHT[2];
  float_t ConValHum;
  float_t ConValLux;
}SensorConvertedValues;
extern SensorConvertedValues ConvertedValues; 

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
  SenMeasStates SenMeasTaskState;
}TaskStateMng;
extern TaskStateMng TaskState;

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