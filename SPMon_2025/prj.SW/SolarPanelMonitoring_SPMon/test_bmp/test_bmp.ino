#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp180;

void setup()
{
    Serial.begin(115200);
    Wire.begin();

    if (!bmp180.begin())
    {
        Serial.println("BMP180 NOT FOUND");
        while (1);
    }
}

void loop()
{
    float temperature = bmp180.readTemperature();
    int32_t pressure  = bmp180.readPressure();
    float altitude    = bmp180.readAltitude();

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println(" Pa");

    Serial.print("Altitude: ");
    Serial.print(altitude);
    Serial.println(" m");

    Serial.println("--------------------");
    delay(2000);
}
