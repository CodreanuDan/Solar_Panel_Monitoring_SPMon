/* 
 * SPMon_Slave.ino
 * Measures parameters on the PV Panel and sends data via BLE to ESP32 Master
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

/*============= DEBUG SETTINGS =============*/
#define ENABLE_DISABLE_DEBUG true /* true: ENABLE; false: DISABLE*/
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

/* Timer variables (no volatile — not needed with FreeRTOS, and millis() is ISR-safe on ESP32) */

// --- Task timers
#define TIMER_INTERVAL 5000
unsigned long lastTimeC0 = 0;
unsigned long lastTimeC1 = 0;

// --- DS18B20 measurement timing
#define TIMER_INTERVAL_DS18B20 750
unsigned long lastTime_DS18B20 = 0;
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

/* Mutex to protect the shared payload struct between Task_C0 (writer) and Task_C1 (reader) */
SemaphoreHandle_t payloadMutex = nullptr;

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

/* Increased stack sizes:
 *   Task_C0 does I2C reads on DHT, BMP180, BH1750, AS7341 — libraries need stack room.
 *   Task_C1 calls BLE notify which also uses internal stack.
 */
TaskDefinition taskList[] = 
{
    { Task_C0, "Task_C0", 8192, NULL, 10, &Task_C0_handle, ESP_CORE_0 },
    { Task_C1, "Task_C1", 8192, NULL, 10, &Task_C1_handle, ESP_CORE_1 }
};


void setup() 
{
    Serial.begin(115200);
    delay(500);

    /* ---------- I2C bus first — every I2C sensor needs this ---------- */
    Wire.begin();
    delay(300);

    /* ---------- Init all sensors (was commented out — the main crash cause) ---------- */
    I2C_PerhipInitCheck();   // inits AS7341, BH1750, BMP180
    dht22.begin();           // was commented out
    delay(300);

    /* ---------- BLE server (after sensors, so Serial logs from sensor init are visible) ---------- */
    BLE_ServerMgrHdl::createServer();
    delay(300);

    /* ---------- Create the mutex before any task runs ---------- */
    payloadMutex = xSemaphoreCreateMutex();
    if (payloadMutex == nullptr)
    {
        Serial.println(">>> FATAL: Could not create payloadMutex");
        while (1) { delay(1000); }
    }

    /* ---------- Zero out payload ---------- */
    memset(&payload, 0, sizeof(payload));

    /* ---------- Create tasks ---------- */
    for (auto &task : taskList)
    {
        BaseType_t result = xTaskCreatePinnedToCore(
            task.taskFunction,
            task.taskName,
            task.stackSize,
            task.params,
            task.priority,
            task.handle,
            task.core
        );
        if (result != pdPASS)
        {
            Serial.printf(">>> FATAL: Could not create task '%s'\n", task.taskName);
            while (1) { delay(1000); }
        }
    }

    Serial.println(">>> setup() done.");
}

void loop() 
{
    /* Everything runs in tasks — nothing needed here */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


/* ======================== Task_C0 — Sensor readings (Core 0) ======================== */
void Task_C0(void* pvParameters)
{
    for (;;)
    {
        if (millis() - lastTimeC0 > TIMER_INTERVAL)
        {
            /* Lock payload while we write to it */
            if (xSemaphoreTake(payloadMutex, pdMS_TO_TICKS(100)))
            {
                payload.cnt++;

                // --- Read sensors ---
                Task_C0_MeasurmentHdl();

                // --- Print all sensor data ---
                #if ENABLE_DISABLE_DEBUG == true
                Task_C0_PrintDataHdl();
                #endif

                xSemaphoreGive(payloadMutex);
            }
            else
            {
                Serial.println(">>> Task_C0: Could not acquire payloadMutex (timeout)");
            }

            lastTimeC0 = millis();
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/* ======================== Task_C1 — BLE send (Core 1) ======================== */
void Task_C1(void* pvParameters)
{
    for (;;)
    {   
        if (millis() - lastTimeC1 > TIMER_INTERVAL * 2)
        {
            /* Lock payload while we read it for BLE */
            if (xSemaphoreTake(payloadMutex, pdMS_TO_TICKS(100)))
            {
                BLE_ServerMgrHdl::sendPayload((uint8_t*)&payload, sizeof(payload));
                xSemaphoreGive(payloadMutex);
            }
            else
            {
                Serial.println(">>> Task_C1: Could not acquire payloadMutex (timeout)");
            }

            lastTimeC1 = millis();
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}


/* ======================== Measurement handler ======================== */
void Task_C0_MeasurmentHdl()
{
    /* --- DHT22 --- */
    float temp_dht_read = dht22.readTemperature();
    float hum_dht_read  = dht22.readHumidity();

    if (!isnan(temp_dht_read)) 
    {
        payload.tempDHT = temp_dht_read;
    } 
    else 
    {
        Serial.println(">>> DHT22: Temperature read failed (NaN)");
    }

    if (!isnan(hum_dht_read)) 
    {
        payload.humDHT = hum_dht_read;
    } 
    else 
    {
        Serial.println(">>> DHT22: Humidity read failed (NaN)");
    }

    /* --- DS18B20 (async conversion) --- */
    DS18B20_MeasWrapper();

    /* --- BMP180 pressure (readPressure returns int32_t in Pa) --- */
    int32_t pressure_read = bmp180.readPressure();
    float bmpTemp = bmp180.readTemperature();
    if (pressure_read > 0) 
    { 
        payload.pressure = (float)pressure_read / 100.0f;  // convert Pa → hPa
        payload.tempBmp = bmpTemp;
    } 
    else 
    {
        Serial.println(">>> BMP180: Pressure Temp read failed");
    }

    /* --- BH1750 lux --- */
    float lux_read = bh1750.readLightLevel();
    if (lux_read >= 0.0f) 
    {
        payload.lux = (uint16_t)lux_read;
    } 
    else 
    {
        Serial.println(">>> BH1750: Lux read failed");
    }

    /* --- AS7341 spectral --- */
    AS7341_MeasWrapper();
}

/* ======================== Print handler ======================== */
void Task_C0_PrintDataHdl()
{
    Serial.println("=== Sensor readings ===");
    Serial.printf("Count: %u\n", payload.cnt);
    Serial.printf("DHT22 Temperature: %.2f °C\n", payload.tempDHT);
    Serial.printf("DHT22 Humidity: %.2f %%\n", payload.humDHT);
    Serial.printf("DS18B20 Temperature: %.2f °C\n", payload.tempDS18B20);
    Serial.printf("BMP180 Pressure: %.2f hPa\n", payload.pressure);
    Serial.printf("BMP180 Temp: %.2f °C\n", payload.tempBmp);
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

/* ======================== I2C Init & Scan ======================== */
void I2C_PerhipInitCheck()
{
    /* Scan I2C Bus */
    I2C_ScanBus();

    // /* Check and setup AS7341 Sensor */
    AS7341_init();

    // /* Check bh1750 sensor */
    if (!bh1750.begin())
    {
        Serial.println(">>> Could not find BH1750");
    }

    /* Check bmp180 sensor */
    if (!bmp180.begin())
    {
        Serial.println(">>> Could not find BMP180");
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

/* ======================== DS18B20 async wrapper ======================== */
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

/* ======================== AS7341 wrapper ======================== */
void AS7341_MeasWrapper()
{
    uint16_t specValues[NUM_CHANNELS] = {0};
    AS7341_getSpectralComponents(specValues);
    memcpy(payload.spec, specValues, sizeof(specValues));
}
