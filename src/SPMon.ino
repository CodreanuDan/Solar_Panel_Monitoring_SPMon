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
  /* Print sensor values and task state */
  Serial.print(F("Loop - Task State: "));
  Serial.print(TaskState.SenMeasTaskState);
  Serial.print(F("; [LOOP]Raw Temperature Value: "));
  Serial.print(RawValues.RawAdc_TempVal_LM35);
  Serial.print(F("; [LOOP]Converted Temperature Value: "));
  Serial.println(ConvertedValues.ConValTempLM35);

  delay(1000); // Delay to avoid flooding the serial output
}