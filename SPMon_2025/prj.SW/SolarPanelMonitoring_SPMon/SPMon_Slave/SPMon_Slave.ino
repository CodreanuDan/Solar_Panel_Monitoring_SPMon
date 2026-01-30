/* 
 * SPMon_Slave.ino
 * Measures parameteres on the PV Panel and sends data via BLE to ESP32 Master
 */
 
/* Includes */
#include <Adafruit_AS7341.h> // Spectral sensor
#include <DS18B20.h>         // Temperature sensor
#include <BH1750.h>          // Light intensity sensor
#include <DHT.h>             // Temperature and humidity sensor
#include <Adafruit_BMP085.h> // Pressure sensor BMP180

#include <Wire.h>            // I2C bus

#include "BLE_ServerMgr_Hdl.h"
#include "SensorPayload.h"  
#include "AS7341_Hdl.h"

/*================ TASKS ===================*/

/* Cores */
typedef enum 
{
    ESP_CORE_0 = 0u,
    ESP_CORE_1
} Cores;

/* Task info struct */
typedef struct
{
    void (*taskFunction)(void*);
    const char* taskName;
    uint32_t stackSize;
    void* params;
    UBaseType_t priority;
    TaskHandle_t* handle;
    Cores core;
} TaskDefinition;

/* Task handles */
TaskHandle_t Task_C0_handle = nullptr;
TaskHandle_t Task_C1_handle = nullptr;

/*=============== SENSORS ===================*/

/* Timer variables */

// --- Task timers
#define TIMER_INTERVAL 5000
volatile unsigned long lastTimeC0 = 0;
volatile unsigned long lastTimeC1 = 0;

// --- DS18B20 measurement timing
#define TIMER_INTERVAL_DS18B20 750
volatile unsigned long lastTime_DS18B20 = 0;
bool ds18b20Busy = false;

/* DHT pin and sensor type */
#define DHTPIN 13
#define DHTTYPE DHT22  

/* Create objects for the sensors and data structures */
SensorPayload payload;
DS18B20 ds18b20(4);
DHT dht22(DHTPIN, DHTTYPE);
BH1750 bh1750; 
Adafruit_BMP085 bmp180;

/* I2C Sensor Init check */
void I2C_PerhipInitCheck();
void I2C_ScanBus();

/* Task function prototypes */
void Task_C0(void* pvParameters);
void Task_C1(void* pvParameters);

/* Task_C0 helper function prototypes */
void Task_C0_MeasurmentHdl();
void Task_C0_PrintDataHdl();

/* DS18B20 measurement wrapper */
void DS18B20_MeasWrapper();
/* AS7341 measurement wrapper */
void AS7341_MeasWrapper();

TaskDefinition taskList[] = 
{
    { Task_C0, "Task_C0", 4096, NULL, 10, &Task_C0_handle, ESP_CORE_0 },
    { Task_C1, "Task_C1", 4096, NULL, 10, &Task_C1_handle, ESP_CORE_1 }
};


#pragma GCC diagnostic ignored "-Wreturn-type"

void setup() 
{
    Serial.begin(115200);
    dht22.begin();
    Wire.begin();
    I2C_PerhipInitCheck();
    BLE_ServerMgrHdl::createServer();

    /* Create tasks */
    for (auto &task : taskList)
    {
        xTaskCreatePinnedToCore(
            task.taskFunction,
            task.taskName,
            task.stackSize,
            task.params,
            task.priority,
            task.handle,
            task.core
        );
    }
}

void loop() 
{
    /* Do nothing */
}


void Task_C0(void* pvParameters)
{
    for (;;)
    {
        if (millis() - lastTimeC0 > TIMER_INTERVAL)
        {
            payload.cnt++;

            // --- Read sensors ---
            Task_C0_MeasurmentHdl();

            // --- Print all sensor data ---
            Task_C0_PrintDataHdl();

            lastTimeC0 = millis();
            
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Small yield
    }
}

void Task_C1(void* pvParameters)
{
    for (;;)
    {   
        if (millis() - lastTimeC1 > TIMER_INTERVAL * 2)
        {
            BLE_ServerMgrHdl::sendPayload((uint8_t*)&payload, sizeof(payload));
            lastTimeC1 = millis();
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Small yield
    }
}


/* Task_C0  */
void Task_C0_MeasurmentHdl()
{
    float temp_dht_read = dht22.readTemperature();
    float hum_dht_read  = dht22.readHumidity();
    float pressure_read = bmp180.readPressure();
    float lux_read      = bh1750.readLightLevel();

    if (!isnan(temp_dht_read)) 
    {
        payload.tempDHT = temp_dht_read;
    } 
    else 
    {
    }

    if (!isnan(hum_dht_read)) 
    {
        payload.humDHT = hum_dht_read;
    } 
    else 
    {
    }

    DS18B20_MeasWrapper();

    if (pressure_read >= 0.0f) 
    { 
        payload.pressure = pressure_read;
    } 
    else 
    {
    }

    if (lux_read >= 0.0f) 
    {
        payload.lux = lux_read;
    } 
    else 
    {
    }

    AS7341_MeasWrapper();
}
void Task_C0_PrintDataHdl()
{
    Serial.println("=== Sensor readings ===");
    Serial.printf("Count: %d\n", payload.cnt);
    Serial.printf("DHT22 Temperature: %.2f °C\n", payload.tempDHT);
    Serial.printf("DHT22 Humidity: %.2f %%\n", payload.humDHT);
    Serial.printf("DS18B20 Temperature: %.2f °C\n", payload.tempDS18B20);
    Serial.printf("BMP180 Pressure: %.2f Pa\n", payload.pressure);
    Serial.printf("BH1750 Light: %u lux\n", payload.lux);

    const char* spectralNames[NUM_CHANNELS] = {
        "Violet", "White", "White-Green", "Green", "Yellow-Green",
        "Yellow", "Orange-Red", "Red", "NIR", "Clear"
    };

    for (int i = 0; i < NUM_CHANNELS; i++) 
    {
        Serial.printf("%s: %u\n", spectralNames[i], payload.spec[i]);
    }
    Serial.println("========================");
}

/* I2C Sensor Init check */
void I2C_PerhipInitCheck()
{
    /* Scan I2C Bus */
    I2C_ScanBus();

    /* Check and setup AS7341 Sensor */
    AS7341_init();

    /* Check bh1750 sensor */
    if (!bh1750.begin())
    {
        Serial.println(">>> Could not find BH1750");
        // while(1);
    }

    /* Check bmp180 sensor */
    if (!bmp180.begin())
    {
        Serial.println(">>> Could not find BMP180");
        // while(1);
    }
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

/* DS18B20 measurement wrapper */
void DS18B20_MeasWrapper()
{
    unsigned long now = millis(); 

    if (!ds18b20Busy)
    {
        ds18b20.doConversion();
        ds18b20Busy = true;
        lastTime_DS18B20 = now;
    }
    else if (now - lastTime_DS18B20 > TIMER_INTERVAL_DS18B20)
    {
        payload.tempDS18B20 = ds18b20.getTempC();
        ds18b20Busy = false;
    }                       
}

/* AS7341 measurement wrapper */
void AS7341_MeasWrapper()
{
    uint16_t specValues[NUM_CHANNELS] = {0};
    AS7341_getSpectralComponents(specValues);
    memcpy(payload.spec, specValues, sizeof(specValues));
}
