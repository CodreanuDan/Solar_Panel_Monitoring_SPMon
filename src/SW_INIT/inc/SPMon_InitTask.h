/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_InitTask.h
* Descr: Contains the header files for SPMon_InitTask.cpp
*
*
*
*
*
* Version: 1.0, 20.10.2024
******************************************************************************************************/
#ifndef SPMON_INITTASK_H
#define SPMON_INITTASK_H

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"

/******************************************************************************************************
* Function name: SPMon_InitTask_MainFunc
* Descr: Main function for the SPMon_InitTask protoype
* Params:pointer to TaskStateMng struct
*
******************************************************************************************************/
void SPMon_InitTask_MainFunc(void *parameter);	


/******************************************************************************************************
* Class name: SPMonInitTask
* Descr: Class containing the prototypes for functions needed for setup 
* 
*
******************************************************************************************************/
class SPMonInitTask
{
  public:
    static void SPMon_InitTask_CreateInitTask();
    static bool SPMon_InitTask_CheckInitData(void *parameter);
};

/* Declare class object */
extern SPMonInitTask spmonInit;


#endif