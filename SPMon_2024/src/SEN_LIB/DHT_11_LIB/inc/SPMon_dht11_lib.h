/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_dht11_lib.h
* Descr: Contains the declaration of the DHT11 sensor
*
*
*
*
*
* Version: 1.0, 26.11.2024
******************************************************************************************************/

/******************************************************************************************************
 * IMPORT AREA
 * ******************************************************************************************************/
#include "SPMon_DataTypes.h"


/******************************************************************************************************
 * Class name: SPMon_DHT11_Sensor_Library 
 * Descr: Class that contains the declaration of the DHT11 sensor function library
 *
 *
 ******************************************************************************************************/
class SPMon_DHT11_Sensor_Library
{
public:
    SPMon_DHT11_Sensor_Library(uint8_t dht11Pin);	 
    void DHT11_GetRawData(SensorRawValues *sensorRawValues);
    void DHT11_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);
private:
    uint8_t _pin;
    byte DHT11_ReadDataByte();  
    void DHT11_StartSequence();
};

extern SPMon_DHT11_Sensor_Library dht11;