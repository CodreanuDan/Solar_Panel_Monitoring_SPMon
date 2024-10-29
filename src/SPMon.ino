/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon.ino
* Descr: Main file of the project, contains the initalisation and setup for the components
*
*@brief: NOTE, we can add built in Hall sensor  ?
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
  /* Empty */
}
