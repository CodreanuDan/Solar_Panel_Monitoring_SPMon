#ifndef SPMON_DHT_11_SENSOR_LIB_H
#define SPMON_DHT_11_SENSOR_LIB_H

#include "Arduino.h"

#define DHT_11_ERROR_CRC 254u
#define DHT_11_ERROR_TIMEOUT 253u
#define DHT_11_TIMEOUT_DURATION 1000u
#define DHT_11_ALERT_SIG_DURATION 18u
#define DHT_11_REQUEST_SIG_DURATION 35u
#define DHT_11_RESPONSE_SIG_DURATION 80u

class SPMon_DHT_11_Sensor_Library
{
    public:
        SPMon_DHT_11_Sensor_Library(int dht_11_pin);
        static int DHT_11_ReadTemeperature;
        static int DHT_11_ReadHumidity;
        static String getErrorString(int errorCode);
        static int DHT_11_ReadAndCheckData();
    private:
        int _dht_11_pin;
        static int DHT_11_ReadRawData(byte data[5]);
        static byte DHT_11_ReadByte();
        static void DHT_11_StartSignal(int dht_11_pin);

};

extern SPMon_DHT_11_Sensor_Library dht_11;

#endif