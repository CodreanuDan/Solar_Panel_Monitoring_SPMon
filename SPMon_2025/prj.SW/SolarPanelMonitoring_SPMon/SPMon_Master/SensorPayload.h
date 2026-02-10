#ifndef SENSOR_PAYLOAD_H
#define SENSOR_PAYLOAD_H

#include <stdint.h>

/* Common payload format between Server and Client */
typedef struct __attribute__((packed)) 
{
    uint16_t cnt;         // 2 bytes
    float tempDHT;        // 4 bytes
    float humDHT;         // 4 bytes
    float tempDS18B20;    // 4 bytes
    float tempBmp;        // 4 bytes
    float pressure;       // 4 bytes
    uint16_t lux;         // 2 bytes
    uint16_t spec[10];    // 10 * 2 = 20 bytes
} SensorPayload;          // Total: 2+4+4+4+4+4+2+20 = 44 bytes

/* Sensor payload expected size */
#define SENSOR_PAYLOAD_SIZE sizeof(SensorPayload)

#endif /* SENSOR_PAYLOAD_H */