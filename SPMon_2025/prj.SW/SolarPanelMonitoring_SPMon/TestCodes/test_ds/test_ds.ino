// #include <DS18B20.h>

// DS18B20 ds18b20(4);

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.println("DS18B20 test started");
// }

// void loop() {
//   ds18b20.doConversion();
//   delay(750);

//   float temp = ds18b20.getTempC();
//   Serial.printf("Temperature: %.2f °C\n", temp);

//   delay(2000);
// }

#include <DS18B20.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// ===== DS18B20 =====
#define DS18B20_PIN 4
DS18B20 ds18b20(DS18B20_PIN);

// ===== BMP180 =====
Adafruit_BMP085 bmp180;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== DS18B20 + BMP180 TEST STARTED ===");

    // I2C init
    Wire.begin();

    // BMP180 init
    if (!bmp180.begin())
    {
        Serial.println("BMP180 NOT FOUND");
        while (1);
    }
    Serial.println("BMP180 initialized");

    Serial.println("DS18B20 initialized");
    Serial.println("----------------------------------");
}

void loop()
{
    // ===== DS18B20 =====
    ds18b20.doConversion();
    delay(750);  // timp necesar conversiei

    float temp_ds = ds18b20.getTempC();

    // ===== BMP180 =====
    float temp_bmp = bmp180.readTemperature();
    int32_t pressure = bmp180.readPressure();
    float altitude = bmp180.readAltitude();

    // ===== OUTPUT =====
    Serial.println("Sensor readings:");

    Serial.printf("DS18B20 Temperature: %.2f °C\n", temp_ds);

    Serial.printf("BMP180 Temperature : %.2f °C\n", temp_bmp);
    Serial.printf("BMP180 Pressure    : %ld Pa\n", pressure);
    Serial.printf("BMP180 Altitude    : %.2f m\n", altitude);

    Serial.println("----------------------------------");
    delay(2000);
}
