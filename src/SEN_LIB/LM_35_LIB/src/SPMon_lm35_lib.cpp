/******************************************************************************************************
* Project name: Solar Panel Monitoring
* Author: Codreanu Dan
* File name: SPMon_lm35_lib.cpp
* Descr: Contains the implementation of the LM35 sensor
*
*
*
*
*
* Version: 1.0, 29.10.2024
******************************************************************************************************/

/******************************************************************************************************
* IMPORT AREA 
******************************************************************************************************/
#include "SPMon_DataTypes.h"
#include "SPMon_lm35_lib.h"

/******************************************************************************************************
 * Function name: LM35_GetTemp
 * Descr: Function that converts the raw data from the LM35 sensor to temperature
 *        Read the raw data from the struct RawValues and convert it to temperature
 *        Convert the raw data to milivolts
 *        Compute the final temperature value by taking the average of the samples from oversampling
 * Params: sensorRawValues, sensorConvertedValues, sensorError
 * Return: 
 *  
 *******************************************************************************************************/
void SPMon_LM35_Sensor_Library::LM35_GetTemp(SensorRawValues * sensorRawValues, SensorConvertedValues * sensorConvertedValues)
{
    Serial.println("LM35_GetTemp started");
    /* Convert the raw data to milivolts */
    float miliVolts_adcConv = (sensorRawValues->RawAdc_TempVal_LM35 * POWER_SUPPLY_VOLTAGE_3V3) / ADC_MAX_VAL;
    float miliVolts = analogReadMilliVolts(ADC_PORT_LM_35);
    /* Convert the milivolts to temperature */
    if ((miliVolts_adcConv + miliVolts) / 2 > ADC_MIN_VAL && (miliVolts_adcConv + miliVolts) / 2 < ADC_MAX_VAL)
    {
        /* Convert the raw data from the ADC to temperature in Celsius */
        float sampleValue = (miliVolts / CONV_DIVISOR) + LM35_CALIBRATION_OFFSET;
        /* Array to store the average values */
        uint8_t avg_val[LM35_MAX_SAMPLES];
        /* Variable to calculate the average value */
        float avg_temp = 0;
        for (uint8_t i = 0; i < LM35_MAX_SAMPLES; i++)
        {
            avg_val[i] = sampleValue;
            avg_temp += avg_val[i];
        }
        /* Calculate the average value */
        avg_temp = avg_temp / LM35_MAX_SAMPLES;
        /* Check if the temperature is in the range */
        if (avg_temp > MAX_VAL_TEMP || avg_temp < MIN_VAL_TEMP)
        {
        }
        else
        {
            sensorConvertedValues->ConValTempLM35 = avg_temp;
        }

    }
    else if ((miliVolts_adcConv + miliVolts) / 2 < ADC_MIN_VAL)
    {

    }
    else if ((miliVolts_adcConv + miliVolts) / 2 > ADC_MAX_VAL)
    {

    }
    
    // Serial.println(sensorConvertedValues->ConValTempLM35); 
    // Serial.println("LM35_GetTemp ended");

}

/******************************************************************************************************
 * Function name: LM35_Calib
 * Descr: Function that calibrates the LM35 sensor
 * Params: 
 * Return: 
 *  
 *******************************************************************************************************/
void SPMon_LM35_Sensor_Library::LM35_Calib(uint8_t sensorPin, uint8_t adcResolution, uint8_t adcAttenuation)
{   
    Serial.println("LM35 Calibration started");
    /* Mark pin as input */
    pinMode(sensorPin, INPUT);
    /* Attach the LM35 sensor pin to the ADC */
    adcAttachPin(sensorPin);
    /* Set the ADC resolution to 12 bits */
    analogReadResolution(adcResolution);
    /* Set the ADC attenuation  for the LM35 sensor pin
    ADC_0db:   ~~> 1.1V
    ADC_2_5db: ~~> 1.5V
    ADC_6db:   ~~> 2.2V
    ADC_11db:  ~~> 3.3V */
    /* {!!!} use static cast for adcAttenuation beacuse the func expects elements from adc_attenuation_t enum */
    analogSetPinAttenuation(sensorPin, static_cast<adc_attenuation_t>(adcAttenuation));
    Serial.println("LM35 Calibration ended");
}

/******************************************************************************************************
 * Function name: LM35_GetRawData
 * Descr: Function that converts the raw data from the LM35 sensor to temperature
 *        Read raw data from adc port using analogRead and setting for the ADC data aquisition
 * Params: sensorRawValues
 * Return: 
 *  
 *******************************************************************************************************/
void SPMon_LM35_Sensor_Library::LM35_GetRawData(SensorRawValues * sensorRawValues)
{

    Serial.println("LM35_GetRawData started");
    /* Read the raw data from the LM35 sensor */
    uint16_t adcData = analogRead(ADC_PORT_LM_35);
    sensorRawValues->RawAdc_TempVal_LM35 = adcData;
    Serial.println(sensorRawValues->RawAdc_TempVal_LM35); 
    Serial.println("LM35_GetRawData ended");
     
}
