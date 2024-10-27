/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_MainTask.h
* Descr: Contains the functions for SPMon_MainTask
*
*
*
*
*
* Version: 1.0, 26.10.2024
******************************************************************************************************/
#ifndef SPMON_MAINTASK_H
#define SPMON_MAINTASK_H

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"

/******************************************************************************************************
* Function name: SPMon_MainTask_MainFunc
* Descr: Main function for the SPMon_MainTask_MainFunc protoype
* Params: none
*
******************************************************************************************************/
void SPMon_MainTask_MainFunc(void *parameter);	

/******************************************************************************************************
* Class name: class SPMonMainTask
* Descr: Class containing the prototypes for functions needed for the main task
* 
*
******************************************************************************************************/
class SPMonMainTask
{
  public:
    static void SPMon_MainTask_CreateMainTask();
    static void SPMon_MainTask_ExecuteStateLogic(InitFlags * InitFlag);
};

/* Declare class object */
extern SPMonMainTask spmonMain;


#endif