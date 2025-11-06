
#include <Adafruit_AS7341.h> // Spectral sensor
#include <DS18B20.h>         // Temperature sensor
#include <BH1750.h>          // Light intensity sensor
#include <DHT.h>             // Temperature and humidity sensor

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
#define TIMER_INTERVAL 5000
volatile unsigned long lastTimeC0 = 0;
volatile unsigned long lastTimeC1 = 0;

/* DHT pin and sensor type */
#define DHTPIN 13
#define DHTTYPE DHT22  

/* Create objects for the sensors and data structures */
SensorPayload payload;
DS18B20 ds18b20(23);
DHT dht22(DHTPIN, DHTTYPE);
BH1750 bh1750; 

/* Task function prototypes */
void Task_C0(void* pvParameters);
void Task_C1(void* pvParameters);

TaskDefinition taskList[] = 
{
    { Task_C0, "Task_C0", 4096, NULL, 10, &Task_C0_handle, ESP_CORE_0 },
    { Task_C1, "Task_C1", 4096, NULL, 10, &Task_C1_handle, ESP_CORE_1 }
};


#pragma GCC diagnostic ignored "-Wreturn-type"

void setup() 
{
    Serial.begin(115200);
    Wire.begin();
    AS7341_init();
    bh1750.begin();
    dht22.begin();
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
            payload.tempDHT = dht22.readTemperature();
            payload.humDHT  = dht22.readHumidity();
            ds18b20.doConversion();payload.tempDS18B20 = ds18b20.getTempC();
            payload.lux = bh1750.readLightLevel();
            
            uint16_t specValues[NUM_CHANNELS] = {0};
            AS7341_getSpectralComponents(specValues);
            memcpy(payload.spec, specValues, sizeof(specValues));

            // --- Print all sensor data ---
            Serial.println("=== Sensor readings ===");
            Serial.printf("Count: %d\n", payload.cnt);
            Serial.printf("DHT22 Temperature: %.2f °C\n", payload.tempDHT);
            Serial.printf("DHT22 Humidity: %.2f %%\n", payload.humDHT);
            Serial.printf("DS18B20 Temperature: %.2f °C\n", payload.tempDS18B20);
            Serial.printf("BH1750 Light: %u lux\n", payload.lux);

            const char* spectralNames[NUM_CHANNELS] = {
                "Violet", "White", "White-Green", "Green", "Yellow-Green",
                "Yellow", "Orange-Red", "Red", "NIR", "Clear"
            };

            for (int i = 0; i < NUM_CHANNELS; i++) {
                Serial.printf("%s: %u\n", spectralNames[i], payload.spec[i]);
            }
            Serial.println("========================");

            lastTimeC0 = millis();
            
        }

        vTaskDelay(10 / portTICK_PERIOD_MS); // Small yield
    }
}

void Task_C1(void* pvParameters)
{
    for (;;)
    {   
        if (millis() - lastTimeC1 > TIMER_INTERVAL)
        {
            BLE_ServerMgrHdl::sendPayload((uint8_t*)&payload, sizeof(payload));
            lastTimeC1 = millis();
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // Small yield
    }
}