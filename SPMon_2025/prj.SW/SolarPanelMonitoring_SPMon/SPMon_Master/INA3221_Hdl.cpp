#include "INA3221_Hdl.h"
#include <Arduino.h>

/* Sensor object */
INA3221 ina3221(INA3221_ADDR40_GND);
/* Unified payload project*/
INA3221_Measurements ina3221_payload; 

void INA3221_init() 
{
    ina3221.begin(&Wire);
    ina3221.reset();
    ina3221.setShuntRes(SHUNT_RESISTOR_VALUE, SHUNT_RESISTOR_VALUE, SHUNT_RESISTOR_VALUE);
    
    Serial.println(">>> INA3321 Initialized and configured.");
}

void INA3221_measureUIperChannels(INA3221_Measurements* data)
{
    /*
     *  Current value is multiplied by 1000 cosidering coversion form A to mA
     */ 
    
    /* INA3321 CH1 */
    data->current_ch1 = ina3221.getCurrent(INA3221_CH1) * 1000.0;
    data->voltage_ch1 = ina3221.getVoltage(INA3221_CH1);
    
    /* INA3321 CH2 */
    data->current_ch2 = ina3221.getCurrent(INA3221_CH2) * 1000.0;
    data->voltage_ch2 = ina3221.getVoltage(INA3221_CH2);
    
    /* INA3321 CH3 */
    data->current_ch3 = ina3221.getCurrent(INA3221_CH3) * 1000.0;
    data->voltage_ch3 = ina3221.getVoltage(INA3221_CH3);

    //Serial.printf(">>> CH1: I=%.2f mA, U=%.2f V\n", data->current_ch1, data->voltage_ch1);
}