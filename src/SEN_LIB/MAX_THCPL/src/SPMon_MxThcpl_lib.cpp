/******************************************************************************************************
 * Project name: Solar Panel Monitoring
 * Author: Codreanu Dan
 * File name: SPMon_MxThcpl_lib.cpp
 * Descr: Contains the function definitions for the MAX_THCPL library
 *
 *
 *
 *
 *
 * Version: 1.0, 9.11.2024
 ******************************************************************************************************/

/******************************************************************************************************
 * IMPORT AREA
 ******************************************************************************************************/
#include "SPMon_MxThcpl_lib.h"

/******************************************************************************************************
 * Function definitions
 ******************************************************************************************************/

/******************************************************************************************************
 * Class Constructor: MAX6675_Calib
 * Descr: Constructor for the MAX6675_Calib class, will be ued in calib task to initialize the sensor
 * Params: SCLK, CS, MISO
 * Return:
 *
 *******************************************************************************************************/
SPMon_MAX6675_THCPL_Sensor_Library::SPMon_MAX6675_THCPL_Sensor_Library(uint8_t SCLK, uint8_t CS, uint8_t MISO)
{
    /* Initialize the pins */
    sclk = SCLK;
    cs = CS;
    miso = MISO;

    /* define pin modes */
    pinMode(cs, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(miso, INPUT);

    digitalWrite(cs, HIGH);
}


/******************************************************************************************************
 * Function name: MAX6675_GetRawDataSequence
 * Descr: Function that reads the raw data from the MAX6675 sensor (SPI Raw constructed frame)
 *        It computes the raw data by reading the data frame from the sensor and storing it in the struct RawValues
 *        The data is consisiting of 16 bits, made up from 2 bytes (MSB and LSB) wich are 2 SPI raw frames
 *        Shift the MSB 8 bits to the left and add the LSB to get the full 16 bit data
 * Params:
 * Return: byte
 *
 *******************************************************************************************************/
byte SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetRawDataSequence()
{
    /* Variable to store the raw data */
    uint16_t SPIrawValue;

    /* Request data from the MAX6675, start SPI communication by setting the cs pin to LOW */
    digitalWrite(cs, LOW);
    delayMicroseconds(10);

    /* Read the first SPI frame */
    SPIrawValue = SPI_ReadRawFrame();

    /* Shift the MSB 8 bits to the left and add the LSB to get the full 16 bit data */
    SPIrawValue <<= 8;

    /* Read the second SPI frame and add it to the first frame using the OR operator */
    SPIrawValue |= SPI_ReadRawFrame();

    /* Stop SPI communication by setting the cs pin to HIGH */
    digitalWrite(cs, HIGH);

    /* Check if the thermocouple is attached */
    if (SPIrawValue & 0x4)
    {
        Serial.println("Error: Thermocouple not attached!");
    }

    /* Remove the 3 bits that are not needed */
    SPIrawValue >>= 3;

    return SPIrawValue;
}

/******************************************************************************************************
 * Function name: MAX6675_GetRawData
 * Descr: Function that reads the raw data from the MAX6675 sensor (SPI Raw constructed frame)
 *        Single read or with oversampling rate defined by THERMOCOUPLE_OVERSAMPLING_RATE
 * Params: sensorRawValues
 * Return:
 *
 *******************************************************************************************************/
void SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetRawData(SensorRawValues *sensorRawValues)
{
    /* Variable to store the raw data */
    uint16_t SPIrawData;
    
#if THCPL_OVERSAMPLING == TRUE
    SPIrawData = 0;
    /* Read the raw data with oversampling */
    for (uint8_t i = 0; i < THERMOCOUPLE_OVERSAMPLING_RATE; i++)
    {
        SPIrawData += MAX6675_GetRawDataSequence();
        delay(100);
    }
    /* Get the average of the raw data */
    SPIrawData /= THERMOCOUPLE_OVERSAMPLING_RATE;
#else
    /* Read the raw data without oversampling */
    SPIrawData = MAX6675_GetRawDataSequence();
#endif

    /* Store the raw data in the struct */
    sensorRawValues->thCplRawData = SPIrawData;
}

/******************************************************************************************************
 * Function name: MAX6675_GetTemp
 * Descr:  Function that converts the raw data from the MAX6675 sensor to temperature
 *         Read the raw data from the struct RawValues and convert it to temperature
 *         Shift 3 bits to the right to get the temperature in Celsius to get rid of State, DevID, ThCPlinput bits (0,1,2)
 *         Multiply the raw data with the conversion factor (0.25) to get the temperature in Celsius
 *         bit 15 is is the sign bit, 0 (dummy) the range is 0-1024 Celsius
 * Params: sensorRawValues, sensorConvertedValues
 * Return:
 *
 *******************************************************************************************************/
void SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    /*Local variables to store the raw data and the temperature value */
    uint16_t SPI_RawVal = sensorRawValues->thCplRawData;
    float_t  TempVal = 0;
    /* Convert the raw data to temperature */
    TempVal = SPI_RawVal * THERMOCOUPLE_CONVERSION_FACTOR;
    /* Store the temperature in the struct */
    sensorConvertedValues->thCplConvData = TempVal;

}

/******************************************************************************************************
 * Function name: SPI_ReadRawFrame
 * Descr: Function that reads the raw data from the MAX6675 sensor (SPI single raw frame)
 *        The function reads the data frame bit by bit and constructs the raw data
 *        How the MAX6675 works:
 *       - The MAX6675 sends the data frame MSB first
 *       - The data frame is 8 bits long
 *       - We set the sclck pin to LOW to request bit transfer
 *       - We read the data from the miso(Slave Out) pin with a delay of 50 us
 *       - We set the sclk pin to HIGH to signal the stop bit and wait for 50 us
 *       - We repeat the process 8 times to get the full data frame
 * Params:
 * Return: byte
 *
 *******************************************************************************************************/
byte SPMon_MAX6675_THCPL_Sensor_Library::SPI_ReadRawFrame()
{
    byte spi_data_frame = 0;
    for (int8_t i = 7; i >= 0; i--)
    {
        /* Send LOW pulse 10/50 ms to request SPI Frame from MAX6675 */
        digitalWrite(sclk, LOW);
        delayMicroseconds(50);

        /* Read the data frame bit by bit */
        if (digitalRead(miso))
        {
            /* Set the bit to 1 if the miso pin is HIGH */
            spi_data_frame |= (1 << i);
        }
        else
        {

        }

        /* Send LOW pulse 10/50 ms to stop communication with MAX6675 */
        digitalWrite(sclk, HIGH);
        delayMicroseconds(50);
    }

    return spi_data_frame;
}
