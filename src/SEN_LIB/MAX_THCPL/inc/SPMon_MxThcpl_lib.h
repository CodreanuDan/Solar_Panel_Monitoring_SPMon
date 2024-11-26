/******************************************************************************************************
 * Project name: Solar Panel Monitoring
 * Author: Codreanu Dan
 * File name: SPMon_MxThcpl_lib.h
 * Descr: Contains the function prototypes for the MAX_THCPL library
 *
 *
 *
 *
 *
 * Version: 1.0, 9.11 .2024
 ******************************************************************************************************/
#ifndef SPMON_MXTHCPL_LIB_H
#define SPMON_MXTHCPL_LIB_H

/******************************************************************************************************
 * IMPORT AREA
 ******************************************************************************************************/
#include <Arduino.h>
#include "SPMon_DataTypes.h"

/******************************************************************************************************
 * Function prototypes
 ******************************************************************************************************/

/******************************************************************************************************
 * Class name: SPMon_MAX6675_THCPL_Sensor_Library
 * Descr: Class containing the prototypes for functions needed for MAX_THCPL sensor
 *
 *
 ******************************************************************************************************/
class SPMon_MAX6675_THCPL_Sensor_Library
{
public:
    SPMon_MAX6675_THCPL_Sensor_Library(uint8_t SCLK, uint8_t CS, uint8_t MISO);
    void MAX6675_GetRawData(SensorRawValues *sensorRawValues);
    byte MAX6675_GetRawDataSequence();
    void MAX6675_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);

private:
    uint8_t sclk, miso, cs;
    byte SPI_ReadRawFrame();
};

/* Create an instance of the class */
extern SPMon_MAX6675_THCPL_Sensor_Library thermocouple;

#endif 