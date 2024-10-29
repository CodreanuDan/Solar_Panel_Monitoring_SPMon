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
 * Params: sensorRawValues, sensorConvertedValues, sensorError
 * Return: 
 *  
 *******************************************************************************************************/
void SPMon_LM35_Sensor_Library::LM35_GetTemp(SenorRawValues * sensorRawValues, SenorConvertedValues * sensorConvertedValues, SensorErrorMonitoring * sensorError)
{
    /* Convert the raw data to milivolts */
    float miliVolts_adcConv = (sensorRawValues->RawAdc_TempVal_LM35 * POWER_SUPPLY_VOLTAGE_3V3) / ADC_MAX_VAL;
    float miliVolts = analogReadMilliVolts(ADC_PORT_LM_35);
    /* Convert the milivolts to temperature */
    if ((miliVolts_adcConv + miliVolts) / 2 > ADC_MIN_VAL && (miliVolts_adcConv + miliVolts) / 2 < ADC_MAX_VAL)
    {
        /* Convert the raw data from the ADC to temperature in Celsius */
        sampleValue = (miliVolts / CONV_DIVISOR) - LM35_CALIBRATION_OFFSET;
        sensorError->ErrorType = SEN_ERROR_NO_ERROR;
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
        if (avg_temp > MAX_VAL_TEMP || avg_temp < MIN_VAL_TEMP ||avg_temp > LM35_ERROR_THRESHOLD || avg_temp < -LM35_ERROR_THRESHOLD)
        {
            sensorError->SensorType = SENS_TEMP_LM_35;
            sensorError->ErrorType = SEN_ERROR_MEASUREMENT_ERROR;
            sensorError->FaultSensCouter++;
        }
        else
        {
            sensorConvertedValues->ConValTempLM35 = avg_temp;
        }

    }
    else if ((miliVolts_adcConv + miliVolts) / 2 < ADC_MIN_VAL)
    {
        sensorError->SensorType = SENS_TEMP_LM_35;
        sensorError->ErrorType = SEN_ERROR_UNDERVOLATGE;
        sensorError->FaultSensCouter++;
    }
    else if ((miliVolts_adcConv + miliVolts) / 2 > ADC_MAX_VAL)
    {
        sensorError->SensorType = SENS_TEMP_LM_35;
        sensorError->ErrorType = SEN_ERROR_OVERVOLTAGE;
        sensorError->FaultSensCouter++;
    }

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
    /* Attach the LM35 sensor pin to the ADC */
    adcAttachPin(sensorPin);
    /* Set the ADC resolution to 12 bits */
    analogReadResolution(adcResolution);
    /* Set the ADC attenuation  for the LM35 sensor pin
    ADC_0db:   ~~> 1.1V
    ADC_2_5db: ~~> 1.5V
    ADC_6db:   ~~> 2.2V
    ADC_11db:  ~~> 3.3V */
    analogSetPinAttenuation(sensorPin, adcAttenuation);
}

/******************************************************************************************************
 * Function name: LM35_GetRawData
 * Descr: Function that converts the raw data from the LM35 sensor to temperature
 * Params: sensorRawValues
 * Return: 
 *  
 *******************************************************************************************************/
void SPMon_LM35_Sensor_Library::LM35_GetRawData(SenorRawValues * sensorRawValues)
{
    /* Read the raw data from the LM35 sensor */
    uint16_t adcData = analogRead(ADC_PORT_LM_35);
    if( adcData < ADC_MIN_VAL && adcData > ADC_MAX_VAL)
    {
        sensorRawValues->RawAdc_TempVal_LM35 = adcData;
    }
    else
    {

    }
     
}
