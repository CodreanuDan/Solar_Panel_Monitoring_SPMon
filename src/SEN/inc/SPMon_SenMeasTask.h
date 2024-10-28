/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_SenMeasTask.h
* Descr: Contains the functions for SPMon_SenMeasTask
*
*
*
*
*
* Version: 1.0, 28.10.2024
******************************************************************************************************/
#ifndef SPMON_SENMEASTASK_H
#define SPMON_SENMEASTASK_H

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"

/******************************************************************************************************
* Function name: SPMon_SenMeasTask_MainFunc
* Descr: Main function for the SPMon_SenMeasTask_MainFunc protoype
* Params: none
*
******************************************************************************************************/
void SPMon_SenMeasTask_MainFunc(void *parameter);	

/******************************************************************************************************
* Class name: class SPMonMainTask
* Descr: Class containing the prototypes for functions needed for the sensor measurement task
* 
*
******************************************************************************************************/
class SPMon_SensorMeasurementTask
{
  public:
    static void SPMon_SenMeasTask_CreateSenMeasTaskTask();
};

/* Declare class object */
extern SPMon_SensorMeasurementTask sensor_measurement;


#endif