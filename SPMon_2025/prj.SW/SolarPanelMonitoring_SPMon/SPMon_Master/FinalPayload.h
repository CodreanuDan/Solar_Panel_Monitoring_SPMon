#ifndef FINAL_PAYLOAD_H
#define FINAL_PAYLOAD_H

#include <stdint.h>
#include "INA3221_Hdl.h"
#include "SensorPayload.h"

/* Payload format for transmision to Cloud resource */
typedef struct __attribute__((packed)) 
{
  SensorPayload sensorData;
  INA3221_Measurements powerData;
} FinalPayload;

/* Final payload expected size */
#define FINAL_PAYLOAD_SIZE sizeof(FinalPayload)

#endif /* FINAL_PAYLOAD_H */