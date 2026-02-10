#include <Wire.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp180;

// ===== I2C SCANNER =====
void scanI2C()
{
    Serial.println("🔍 I2C scan started...");
    byte count = 0;

    for (byte address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0)
        {
            Serial.print("✅ I2C device found at 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            count++;
        }
    }

    if (count == 0)
        Serial.println("❌ No I2C devices found");
    else
        Serial.println("🔎 I2C scan done");

    Serial.println("--------------------");
}

void setup()
{
    Serial.begin(115200);
    delay(3000);   // ⏱ delay înainte de orice test

    Serial.println("=== SYSTEM BOOT ===");

    Wire.begin();

    // Scanare I2C
    scanI2C();

    // Init BMP180
    if (!bmp180.begin())
    {
        Serial.println("❌ BMP180 NOT FOUND");
        while (1);
    }

    Serial.println("✅ BMP180 initialized");
    Serial.println("--------------------");
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
