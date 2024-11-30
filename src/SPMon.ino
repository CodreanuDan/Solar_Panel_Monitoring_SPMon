/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon.ino
* Descr: Main file for the Solar Panel Monitoring project
*
*
*
*
*
* Version: 1.0, 19.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_InitTask.h"
#include "SPMon_SensCalibTask.h"
#include "SPMon_ComInitTask.h"
#include "SPMon_SenMeasTask.h"

/******************************************************************************************************
* Global Variables
******************************************************************************************************/
SensorRawValues RawValues;
SensorConvertedValues ConvertedValues;
TaskStateMng TaskState;

/******************************************************************************************************
* Function name: setup
* Descr: Init. of the APP 
* Params:
*
******************************************************************************************************/
void setup() 
{
  /* To use serial monitor*/
  Serial.begin(115200);
  /* Initialize global variables */
  // globalTaskState.SenMeasTaskState = SENS_MEAS_STATE_OFF;
  /* APP startup*/
  spmonInit.SPMon_InitTask_CreateInitTask();
}

/******************************************************************************************************
* Function name: loop
* Descr: not used, due to the fact that we use tasks
* Params:
*
******************************************************************************************************/
void loop() 
{
#if DBG == TRUE
  SPMon_Debbuger();
#endif

  delay(1000); // Delay to avoid flooding the serial output
}

/******************************************************************************************************
 * Function name: SPMon_Debbuger
 * Descr: Function that prints the task state and the sensor values
 * Params:
 * Return:
 *  
 * *******************************************************************************************************/
void SPMon_Debbuger()
{
  /* Print sensor values and task state */
  Serial.print(F("[LOOP] - Task State: "));
  Serial.print(TaskState.SenMeasTaskState);
  Serial.println();
/* LM35 sensor Debug enable: TRUE/FALSE */
#if LM35_SENSOR_ENABLE == TRUE
  Serial.print(F("; [LM35]Raw Temperature Value: "));
  Serial.print(RawValues.RawAdc_TempVal_LM35);
  Serial.print(F("; Converted Temperature Value: "));
  Serial.print(ConvertedValues.ConValTempLM35);
  Serial.println();
#endif
/* THCPL sensor Debug enable: TRUE/FALSE */
#if THCPL_SENSOR_ENABLE == TRUE
  Serial.print(F("; [THCPL]Raw SPI Frame: "));
  Serial.print(RawValues.thCplRawData, BIN);
  Serial.print(F("; [THCPL]Raw SPI Frame(int): "));
  Serial.print(RawValues.thCplRawData);
  Serial.print(F("; Converted SPI Frame: "));
  Serial.print(ConvertedValues.thCplConvData);
  Serial.println();
#endif
/* DHT11 sensor Debug enable: TRUE/FALSE */
#if DHT11_SENSOR_ENABLE == TRUE
  Serial.print(F("; [DHT11]Raw Data: "));
  for (int i = 0; i < 5; i++)
  {
    Serial.print(RawValues.dhtRawData[i]);
    Serial.print(F(" "));
  }
  Serial.println();
#endif
}