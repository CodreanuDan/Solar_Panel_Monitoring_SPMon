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
        static void LM35_GetTemp();
        static void LM35_Calib();
    private:
        static void LM35_GetRawData(SenorRawValues * sensorRawValues);
}


#endif