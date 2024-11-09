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
* Class name: class SPMon_SensorMeasurementTask
* Descr: Class containing the prototypes for functions needed for the sensor measurement task
* 
*
******************************************************************************************************/
class SPMon_SensorMeasurementTask
{
  public:
    static bool SPMon_SenMeasTask_CreateSenMeasTaskTask();
    static void SPMon_SenMeasTask_ExecuteStateMachine(TaskStateMng *taskState, SensorRawValues *rawValues, SensorConvertedValues *convertedValues);
  private:
    static bool taskCreated;
};

/* Declare class object */
extern SPMon_SensorMeasurementTask sensor_measurement;

/******************************************************************************************************
* Class name: class SPMon_SensorMeasurementTask_Interface
* Descr: Interface class containing the prototypes for functions needed for the sensor measurement task
*        - contains generic funtion prototypes so that we can use them to create a pointer to the class
*          and call the functions from the sensor libraries in a vector of function pointers in the func
*          SPI_SensorMeasurementTask_ExecuteStateMachine.
* 
*
******************************************************************************************************/
class SPMon_SensorMeasurementTask_Interface
{
  public:
    virtual void SPMon_SenMeasTask_GetRawData(SensorRawValues * rawValues) = 0;
    virtual void SPMon_SenMeasTask_ConvertData(SensorRawValues * rawValues, SensorConvertedValues * convertedValues) = 0;
    virtual void SPMon_SenMeasTask_ErrorMonitor() = 0;
    /* Class destructor */
    virtual ~SPMon_SensorMeasurementTask_Interface() {};
};

/****************************************************************************************************** 
* Class name: class SPMon_SensorMeasurementTask_LM35
* Descr: Derived class containing the prototypes for functions needed for the sensor measurement task
*        - contains the functions for the LM35 sensor
*        - inherits from the SPMon_SensorMeasurementTask_Interface class
*        - implements the pure virtual functions from the SPMon_SensorMeasurementTask_Interface class
*
/******************************************************************************************************/
class SPMon_SensorMeasurementTask_LM35: public SPMon_SensorMeasurementTask_Interface
{
  public:
    SPMon_SensorMeasurementTask_LM35();
    void SPMon_SenMeasTask_GetRawData(SensorRawValues * rawValues) override; 
    void SPMon_SenMeasTask_ConvertData(SensorRawValues * rawValues, SensorConvertedValues * convertedValues) override;
    void SPMon_SenMeasTask_ErrorMonitor() override;
};

extern SPMon_SensorMeasurementTask_LM35 senMeaslm35;

/******************************************************************************************************/
#endif  /* SPMON_SENMEASTASK_H */ 