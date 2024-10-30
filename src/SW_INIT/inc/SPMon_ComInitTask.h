/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_ComInitTask.h
* Descr:  Contains the header files for SPMon_ComInitTask
*
*
*
*
*
* Version: 1.0, 20.10.2024
******************************************************************************************************/
#ifndef SPMON_COMINITTASK_H
#define SPMON_COMINITTASK_H

/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/
extern TaskHandle_t SPMon_ComInitTask_hdl;

/******************************************************************************************************
* Function name: SPMon_ComInitTask_MainFunc
* Descr: Main function for the SPMon_ComInitTask_MainFunc protoype
* Params: parameter(unused)
*
******************************************************************************************************/
void SPMon_ComInitTask_MainFunc(void *parameter);

/******************************************************************************************************
* Class name: SPMonComInitTask
* Descr: Class containing the prototypes for functions needed for communication setup 
* 
*
******************************************************************************************************/
class SPMonComInitTask
{
  public:
    static void SPMon_ComInitTask_CreateComInitTask();
    static void SPMon_ComInitTaks_ComInitFunction(InitFlags *InitFlag);
  private:
    static void SPMon_ComInitTaks_EstablishWiFiComm();
    static void SPMon_ComInitTaks_CheckComInit();
};

/* Declare class object */
extern SPMonComInitTask comInit; 

#endif