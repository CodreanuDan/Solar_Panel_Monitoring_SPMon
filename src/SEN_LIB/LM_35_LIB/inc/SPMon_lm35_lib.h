/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_lm35_lib.h
* Descr: Contains the header files for LM35 sensor
*
*
*
*
*
* Version: 1.0, 29.10.2024
******************************************************************************************************/

#ifndef SPMON_LM35_LIB_H
#define SPMON_LM35_LIB_H
/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include <Arduino.h>

/******************************************************************************************************
* Class name: class SPMon_LM35_Sensor_Library
* Descr: Class containing the prototypes for functions needed for LM35 sensor
* 
*
******************************************************************************************************/
class SPMon_LM35_Sensor_Library
{
    public:
        static void LM35_GetTemp(SensorRawValues * sensorRawValues, SensorConvertedValues * sensorConvertedValues);
        static void LM35_Calib(uint8_t sensorPin, uint8_t adcResolution, uint8_t adcAttenuation);
        static void LM35_GetRawData(SensorRawValues * sensorRawValues);
    private:
};

/* Creae an instance of the class */
extern SPMon_LM35_Sensor_Library lm35;

/******************************************************************************************************/
#endif /* SPMON_LM35_LIB_H */