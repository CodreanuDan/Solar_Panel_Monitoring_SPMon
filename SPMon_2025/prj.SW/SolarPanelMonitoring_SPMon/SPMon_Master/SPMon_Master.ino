/* 
 * SPMon_Master.ino
 * Measures Iload and Uload and merges with data recevied from ESP32 Slave,
 * sends unified data to Cloud resource.
 */

/* Includes */
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>

#include <Wire.h>
#include <INA3221.h>

#include "BLEDevice.h"
#include "BLE_ClientMgr.h" 
#include "SensorPayload.h"
#include "INA3221_Hdl.h"

/* I2C Sensor Init check */
void I2C_PerhipInitCheck();
void I2C_ScanBus();

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    I2C_PerhipInitCheck();

    /* Init BLE device and start scan */
	  BLE_ClientMgr::init();
}

void loop()
{
    BLE_ClientMgr::communicationManagerMainFunction();
    INA3221_measureUIperChannels(&ina3321_payload);
    delay(1000);
}

/* I2C Sensor Init check */
void I2C_PerhipInitCheck()
{
    /* Scan I2C Bus */
    I2C_ScanBus();
    /* Check and setup AS7341 Sensor */
    INA3321_init();
}

void I2C_ScanBus()
{
    Serial.println(">>> Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) 
    {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) 
        {
            Serial.printf(">>> Found device at 0x%02X\n", addr);
        }
    }
}