/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_dht11_lib.cpp
* Descr: Contains the implementation of the DHT11 sensor
*        -> Docs: https://www.alldatasheet.com/datasheet-pdf/view/1440068/ETC/DHT11.html
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
#include "SPMon_dht11_lib.h"

/******************************************************************************************************
 * Function name: SPMon_DHT11_Sensor_Library
 * Descr: Constructor for the SPMon_DHT11_Sensor_Library class
 *        Initialize the DHT11 sensor pin
 * Params: dht11Pin
 * Return:
 *
 *******************************************************************************************************/
SPMon_DHT11_Sensor_Library::SPMon_DHT11_Sensor_Library(uint8_t dht11Pin) : _pin(dht11Pin)
{
    /* Initialize the pin */
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
}

/******************************************************************************************************
 * Function name: DHT11_GetRawData
 * Descr: Function that reads the raw data from the DHT11 sensor
 *       -> The raw data is stored in the SensorRawValues structure
 *       -> Docs: https://www.alldatasheet.com/datasheet-pdf/view/1440068/ETC/DHT11.html
 *       -> Raw data: 5 bytes --> Humidity (dhtRawData[0], dhtRawData[1]),
 *                                Temperature (dhtRawData[2], dhtRawData[3]),
 *                                CRC (dhtRawData[4]))
 * Params: sensorRawValues
 * Return:
 *
 *******************************************************************************************************/
void SPMon_DHT11_Sensor_Library::DHT11_GetRawData(SensorRawValues *sensorRawValues)
{
    /* Delay for 500ms */
    delay(500);
    /* Start the communication sequence */
    DHT11_StartSequence();
    /* Variable to store the data */
    byte data[5];
    
    /* Check if the sensor has pulled the data line low */
    if (digitalRead(_pin) == LOW)
    {
        /* Wait for the sensor to pull the data line high */
        delayMicroseconds(80);
        /* Check if the sensor has pulled the data line high */
        if (digitalRead(_pin) == HIGH)
        {
            /* Wait for the sensor to pull the data line low */
            delayMicroseconds(80);
            /* Read the data */
            for (uint8_t i = 0; i < 5; i++)
            {
                data[i] = DHT11_ReadDataByte();
            }
            
            /* Check the checksum */
            if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
            {
                /* Store the raw data */
                sensorRawValues->dhtRawData[0] = data[0];
                sensorRawValues->dhtRawData[1] = data[1];
                sensorRawValues->dhtRawData[2] = data[2];
                sensorRawValues->dhtRawData[3] = data[3];
                sensorRawValues->dhtRawData[4] = data[4];
            }
            else
            {

            }
        }
    }

}

/******************************************************************************************************
 * Function name: DHT11_GetConvData
 * Descr: Function that converts the raw data from the DHT11 sensor to temperature and humidity
 *        -> The data is stored in the SensorConvertedValues structure
 *        -> Docs: https://www.alldatasheet.com/datasheet-pdf/view/1440068/ETC/DHT11.html
 *        -> Raw data: 5 bytes --> Humidity (dhtRawData[0], dhtRawData[1]),
 *                                 Temperature (dhtRawData[2], dhtRawData[3]),
 *                                 CRC (dhtRawData[4]))
 *        -> Converted data: Humidity (ConValDHT[0]), Temperature (ConValDHT[1])
 * Params: sensorRawValues, sensorConvertedValues
 * Return: Error code
 *
 *******************************************************************************************************/
void SPMon_DHT11_Sensor_Library::DHT11_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    /* Local variable to store the raw data */
    byte rawData[5];
    /* Get the raw values taken from DHT 11 sensor*/
    memcpy(rawData, sensorRawValues->dhtRawData, sizeof(rawData));

    /* Check if the raw data is not null */
    if(rawData != nullptr)
    {
        /* Convert the raw data to temperature and humidity */
        float humidity = (float)rawData[0] + (float)rawData[1] / 10;        
        float temperature = (float)rawData[2] + (float)rawData[3] / 10;
        /* Add the humidity and temperature values to the converted values */
        sensorConvertedValues->ConValDHT[0] = humidity;
        sensorConvertedValues->ConValDHT[1] = temperature;
    }
    else
    {
        Serial.println(F("[ERROR] Null pointer passed to DHT11_GetConvData"));
    }
}

/******************************************************************************************************
 * Function name: DHT11_ReadDataByte
 * Descr: Function that reads the data byte from the DHT11 sensor
 *        -> Data byte is read bit by bit at a interval of 30us
 *        -> If the data line is high for 30us, the bit is set to 1
 *        -> Docs: https://www.alldatasheet.com/datasheet-pdf/view/1440068/ETC/DHT11.html
 * Params:
 * Return: byte
 *
 *******************************************************************************************************/
byte SPMon_DHT11_Sensor_Library::DHT11_ReadDataByte()
{
    /* Variable to store the data */
    byte data = 0;
    /* Read the data bit by bit for each data byte */
    for (int i = 0; i < 8; i++)
    {
        /* Wait for the sensor to pull the data line low and wait
          for 30us to detect the bit transmission from the sensor */
        while (digitalRead(_pin) == LOW);
        delayMicroseconds(30);
        /* Check if the data line is high to signal a LOGIC 1 and a bit transmission */
        if (digitalRead(_pin) == HIGH)
        {
            /* Use the OR operator to shift the bits in the data variable for each position 
               whem the data line is high (LOGIC 1) */
            data |= (1 << (7 - i));
            while (digitalRead(_pin) == HIGH);
        }
    }
    return data;
}

/******************************************************************************************************
 * Function name: DHT11_StartSequence
 * Descr: Function that starts the communication sequence with the DHT11 sensor 
 *        -> Docs: https://www.alldatasheet.com/datasheet-pdf/view/1440068/ETC/DHT11.html
 *        -> The communication sequence is started by the MCU by pulling the data line low for 18ms
 *        -> Then the MCU pulls the data line high for 40us
 *        -> The MCU sets the data line as input and waits for the sensor to pull the data line low for 80us
 * Params:
 * Return:
 *
 *******************************************************************************************************/
void SPMon_DHT11_Sensor_Library::DHT11_StartSequence()
{
    /* Set the data line as output and send start sequence */
    pinMode(_pin, OUTPUT);
    /* Pull the data line low for 18ms */
    digitalWrite(_pin, LOW);
    delay(18);
    /* Pull the data line high for 40us */
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    /* Set the data line as input and wait for the sensor to 
    begin the communication */
    pinMode(_pin, INPUT);
}