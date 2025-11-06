#ifndef SENSOR_PAYLOAD_H
#define SENSOR_PAYLOAD_H

#include <stdint.h>

/* Common payload format between Server and Client */
typedef struct __attribute__((packed)) 
{
    uint16_t cnt;
    float tempDHT;
    float humDHT;
    float tempDS18B20;
    float tempTHR;
    uint16_t lux;
    uint16_t spec[10];
} SensorPayload;

/* Sensor payload expected size */
#define SENSOR_PAYLOAD_SIZE sizeof(SensorPayload)

#endif /* SENSOR_PAYLOAD_H */