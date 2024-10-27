/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_SensCalibTask.h
* Descr:  Contains the header files for SPMon_SensCalibTask
*
*
*
*
*
* Version: 1.0, 20.10.2024
******************************************************************************************************/
#ifndef SPMON_SENSCALIBTASK_H
#define SPMON_SENSCALIBTASK_H


/******************************************************************************************************
* FILE LOCAL VARIABLES 
******************************************************************************************************/
extern TaskHandle_t SPMon_ComInitTask_hdl;

/******************************************************************************************************
* Function name: SPMon_SensCalibTask_MainFunc
* Descr: Main function for the SPMon_SensCalibTask_MainFunc protoype
* Params: parameter(unused)
*
******************************************************************************************************/
void SPMon_SensCalibTask_MainFunc(void *parameter);


/******************************************************************************************************
* Class name: class SPMonSensCalibTask
* Descr: Class containing the prototypes for functions needed for sensors calibration
* 
*
******************************************************************************************************/
class SPMonSensCalibTask
{
  public:
    static void SPMon_SensCalibTask_CreateSensCalibTask();
    static void SPMon_SensCalibTask_SensCalibFunction(InitFlags *CalibFlag);
  private:
    static void SPMon_SensCalibTask_CalibrateSensors();
    static void SPMon_SensCalibTask_CheckCalibration();
};

/* Declare class object */
extern SPMonSensCalibTask senCal;

#endif