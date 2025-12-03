#ifndef INA3221_HDL_H
#define INA3221_HDL_H

#include <Wire.h>
#include <INA3221.h>

typedef struct __attribute__((packed)) 
{
    float current_ch1;
    float voltage_ch1;
    float current_ch2;
    float voltage_ch2;
    float current_ch3;
    float voltage_ch3;
} INA3221_Measurements;

/* INA3221 sensor object (singleton) ; Set I2C address to 0x41 (A0 pin -> VCC) */
extern INA3221 ina3221;

/* INA3221 payload instance */
extern INA3221_Measurements ina3221_payload; 

/* U-I components idx per channel  */
#define IDX_ICH 0 // Current
#define IDX_UCH 1 // Voltage

#define NUM_CHANNELS 3
#define SHUNT_RESISTOR_VALUE 100u

/* INA3221 payload expected size */
#define INA3221_PAYLOAD_SIZE sizeof(INA3221_Measurements) 

void INA3221_init();
void INA3221_measureUIperChannels(INA3221_Measurements* data);


#endif /* INA3221_HDL_H */