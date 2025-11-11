#ifndef INA3221_HDL_H
#define INA3221_HDL_H

#include <Wire.h>
#include <INA3221.h>

typedef struct __attribute__((packed)) {
    float current_ch1;
    float voltage_ch1;
    float current_ch2;
    float voltage_ch2;
    float current_ch3;
    float voltage_ch3;
} INA3321_Measurements;

/* INA3221 sensor object (singleton) ; Set I2C address to 0x41 (A0 pin -> VCC) */
extern INA3221 ina3321;

/* INA3321 payload instance */
extern INA3321_Measurements ina3321_payload; 

/* U-I components idx per channel (rămân valabile pentru citirea generică) */
#define IDX_ICH 0 // Current
#define IDX_UCH 1 // Voltage

#define NUM_CHANNELS 3
#define SHUNT_RESISTOR_VALUE 100u

/* INA3321 payload expected size */
#define INA3321_PAYLOAD_SIZE sizeof(INA3321_Measurements) // Acum folosește structura

void INA3321_init();
void INA3221_measureUIperChannels(INA3321_Measurements* data);


#endif /* INA3321_HDL_H */