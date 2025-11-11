#include "INA3221_Hdl.h"
#include <Arduino.h>

/* Sensor object */
INA3221 ina3321(INA3221_ADDR40_GND);
/* Unified payload project*/
INA3321_Measurements ina3321_payload; 

void INA3321_init() 
{
    ina3321.begin(&Wire);
    ina3321.reset();
    ina3321.setShuntRes(SHUNT_RESISTOR_VALUE, SHUNT_RESISTOR_VALUE, SHUNT_RESISTOR_VALUE);
    
    Serial.println(">>> INA3321 Initialized and configured.");
}

void INA3221_measureUIperChannels(INA3321_Measurements* data)
{
    /*
     *  Current value is multiplied by 1000 cosidering coversion form A to mA
     */ 
    
    /* INA3321 CH1 */
    data->current_ch1 = ina3321.getCurrent(INA3221_CH1) * 1000.0;
    data->voltage_ch1 = ina3321.getVoltage(INA3221_CH1);
    
    /* INA3321 CH2 */
    data->current_ch2 = ina3321.getCurrent(INA3221_CH2) * 1000.0;
    data->voltage_ch2 = ina3321.getVoltage(INA3221_CH2);
    
    /* INA3321 CH3 */
    data->current_ch3 = ina3321.getCurrent(INA3221_CH3) * 1000.0;
    data->voltage_ch3 = ina3321.getVoltage(INA3221_CH3);

    Serial.printf(">>> CH1: I=%.2f mA, U=%.2f V\n", data->current_ch1, data->voltage_ch1);
}