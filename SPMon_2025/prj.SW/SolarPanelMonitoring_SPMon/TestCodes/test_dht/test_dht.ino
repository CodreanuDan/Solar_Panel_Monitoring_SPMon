// #include <DHT.h>

// #define DHTPIN 13
// #define DHTTYPE DHT22

// DHT dht(DHTPIN, DHTTYPE);

// void setup()
// {
//     Serial.begin(115200);
//     dht.begin();
// }

// void loop()
// {
//     float humidity = dht.readHumidity();
//     float temperature = dht.readTemperature();

//     if (!isnan(humidity) && !isnan(temperature))
//     {
//         Serial.print(temperature);
//         Serial.print(" ");
//         Serial.println(humidity);
//     }

//     delay(100);
// }


#include <DS18B20.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <Adafruit_AS7341.h>

// ================= DS18B20 =================
#define DS18B20_PIN 4
DS18B20 ds18b20(DS18B20_PIN);

// ================= DHT22 ===================
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ================= BMP180 ==================
Adafruit_BMP085 bmp180;

// ================= BH1750 ==================
BH1750 lightMeter;

// ================= AS7341 ==================
Adafruit_AS7341 as7341;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("=== MULTI SENSOR + AS7341 TEST ===");

    // Init magistrala I2C
    Wire.begin();

    // ===== DHT =====
    dht.begin();

    // ===== BMP180 =====
    if (!bmp180.begin())
    {
        Serial.println("❌ BMP180 NOT FOUND");
        while (1);
    }
    Serial.println("✅ BMP180 initialized");

    // ===== BH1750 =====
    if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE))
        Serial.println("✅ BH1750 initialized");
    else
        Serial.println("❌ BH1750 NOT FOUND");

    // ===== AS7341 =====
    if (!as7341.begin())
    {
        Serial.println("❌ AS7341 not found");
        while (1);
    }

    as7341.setATIME(100);
    as7341.setASTEP(999);
    as7341.setGain(AS7341_GAIN_1X);

    Serial.println("✅ AS7341 initialized");
    Serial.println("----------------------------------");
}

void loop()
{
    // ================= DS18B20 =================
    ds18b20.doConversion();
    delay(750);
    float temp_ds = ds18b20.getTempC();

    // ================= DHT22 ===================
    float humidity = dht.readHumidity();
    float temp_dht = dht.readTemperature();

    // ================= BMP180 ==================
    float temp_bmp   = bmp180.readTemperature();
    int32_t pressure = bmp180.readPressure();
    float altitude   = bmp180.readAltitude();

    // ================= BH1750 ==================
    float lux = lightMeter.readLightLevel();

    // ================= AS7341 ==================
    uint16_t spec[10];
    bool spec_ok = as7341.readAllChannels(spec);

    // ================= OUTPUT ==================
    Serial.println("📡 Sensor readings:");

    // DS18B20
    Serial.print("DS18B20_T=");
    Serial.print(temp_ds);
    Serial.println("C");

    // DHT22
    if (!isnan(humidity) && !isnan(temp_dht))
    {
        Serial.print("DHT22_T=");
        Serial.print(temp_dht);
        Serial.print("C H=");
        Serial.print(humidity);
        Serial.println("%");
    }
    else
    {
        Serial.println("DHT22 ERROR");
    }

    // BMP180
    Serial.print("BMP180_T=");
    Serial.print(temp_bmp);
    Serial.print("C P=");
    Serial.print(pressure);
    Serial.print("Pa ALT=");
    Serial.print(altitude);
    Serial.println("m");

    // BH1750
    Serial.print("LIGHT=");
    Serial.print(lux);
    Serial.println(" lux");

    // AS7341
    if (spec_ok)
    {
        Serial.print("AS7341: ");
        for (int i = 0; i < 10; i++)
        {
            Serial.printf("CH%d=%u ", i, spec[i]);
        }
        Serial.println();
    }
    else
    {
        Serial.println("AS7341 READ ERROR");
    }

    Serial.println("----------------------------------");

    delay(2000);   // limitat de DHT22
}





