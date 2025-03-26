#include <Wire.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <INA3221.h>
#include <time.h>

// WiFi credentials
const char *ssid = "SMM";
const char *password = "masurari1";

// Google Apps Script URL
String googleScriptURL = "https://script.google.com/macros/s/AKfycbx6vQJLpcS4Ruqk-a588YPVDlApXlGW83XbxDnTpiHFOaN9KX0l7hNx5EFmX4oqK5Ow/exec";

// BH1750 I2C address
#define BH1750_ADDRESS 0x23

// BH1750 mode
#define BH1750_MODE 0x10

// Number of bytes to read from the sensor
#define BH1750_NR_OF_BYTES 2

// Conversion factor for lux calculation
#define BH1750_CONVERSION_FACTOR 1.2

// DHT11 pin
#define DHT11_PIN 13

// MAX6675 pins
#define MAX6675_SCLK 18
#define MAX6675_CS 5
#define MAX6675_MISO 19

// LM35 pin
#define LM35_PIN 35
#define LM35_MAX_SAMPLES 10

// DSM501A pins
#define DUST_SENSOR_DIGITAL_PIN_PM10 2
#define DUST_SENSOR_DIGITAL_PIN_PM25 4

// Structure to hold raw sensor values
typedef struct
{
    uint16_t RawAdc_LuxVal;
    byte dhtRawData[5];
    uint16_t thCplRawData;
    uint16_t RawAdc_TempVal_LM35;
    long concentrationPM25;
    long concentrationPM10;
} SensorRawValues;

// Structure to hold converted sensor values
typedef struct
{
    float ConValLux;
    float ConValDHT[2];
    float thCplConvData;
    float ConValTempLM35;
} SensorConvertedValues;

// BH1750 sensor library class
class SPMon_BH1750_Sensor_Library
{
public:
    SPMon_BH1750_Sensor_Library(byte addr, byte mode, uint8_t nrOfBytes);
    void BH1750_Init();
    void BH1750_GetRawData(SensorRawValues *sensorRawValues);
    void BH1750_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);

private:
    byte _addr;
    byte _mode;
    uint8_t _nrOfBytes;
    void BH1750_StartSequence();
};

// Constructor
SPMon_BH1750_Sensor_Library::SPMon_BH1750_Sensor_Library(byte addr, byte mode, uint8_t nrOfBytes)
    : _addr(addr), _mode(mode), _nrOfBytes(nrOfBytes) {}

// Initialize the BH1750 sensor
void SPMon_BH1750_Sensor_Library::BH1750_Init()
{
    Serial.println(F("[BH1750] - Init"));
    Wire.begin();
    BH1750_StartSequence();
}

// Start the communication sequence with the BH1750 sensor
void SPMon_BH1750_Sensor_Library::BH1750_StartSequence()
{
    Serial.println(F("[BH1750] - Start sequence"));
    Wire.beginTransmission(_addr);
    Wire.write(_mode);
    Wire.endTransmission();
}

// Read raw data from the BH1750 sensor
void SPMon_BH1750_Sensor_Library::BH1750_GetRawData(SensorRawValues *sensorRawValues)
{
    uint16_t rawData = 0;
    Wire.beginTransmission(_addr);
    Wire.requestFrom(_addr, _nrOfBytes);
    if (Wire.available() == _nrOfBytes)
    {
        rawData = Wire.read();
        rawData <<= 8;
        rawData |= Wire.read();
        sensorRawValues->RawAdc_LuxVal = rawData;
    }
    Wire.endTransmission();
}

// Convert raw data to lux
void SPMon_BH1750_Sensor_Library::BH1750_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    uint16_t rawData = sensorRawValues->RawAdc_LuxVal;
    float lux = (float)rawData / BH1750_CONVERSION_FACTOR;
    sensorConvertedValues->ConValLux = lux;
}

// DHT11 sensor library class
class SPMon_DHT11_Sensor_Library
{
public:
    SPMon_DHT11_Sensor_Library(uint8_t dht11Pin);
    void DHT11_GetRawData(SensorRawValues *sensorRawValues);
    void DHT11_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);

private:
    uint8_t _pin;
    byte DHT11_ReadDataByte();
    void DHT11_StartSequence();
};

// Constructor
SPMon_DHT11_Sensor_Library::SPMon_DHT11_Sensor_Library(uint8_t dht11Pin) : _pin(dht11Pin)
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
}

// Read raw data from the DHT11 sensor
void SPMon_DHT11_Sensor_Library::DHT11_GetRawData(SensorRawValues *sensorRawValues)
{
    delay(500);
    DHT11_StartSequence();
    byte data[5] = {0};

    if (digitalRead(_pin) == LOW)
    {
        delayMicroseconds(80);
        if (digitalRead(_pin) == HIGH)
        {
            delayMicroseconds(80);
            for (uint8_t i = 0; i < 5; i++)
            {
                data[i] = DHT11_ReadDataByte();
            }
            if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
            {
                memcpy(sensorRawValues->dhtRawData, data, sizeof(data));
            }
        }
    }
}

// Convert raw data to temperature and humidity
void SPMon_DHT11_Sensor_Library::DHT11_GetConvData(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    byte rawData[5];
    memcpy(rawData, sensorRawValues->dhtRawData, sizeof(rawData));

    if (rawData != nullptr)
    {
        float humidity = (float)rawData[0] + (float)rawData[1] / 10;
        float temperature = (float)rawData[2] + (float)rawData[3] / 10;
        sensorConvertedValues->ConValDHT[0] = humidity;
        sensorConvertedValues->ConValDHT[1] = temperature;
    }
    else
    {
        Serial.println(F("[ERROR] Null pointer passed to DHT11_GetConvData"));
    }
}

// Read data byte from the DHT11 sensor
byte SPMon_DHT11_Sensor_Library::DHT11_ReadDataByte()
{
    byte data = 0;
    for (int i = 0; i < 8; i++)
    {
        while (digitalRead(_pin) == LOW)
            ;
        delayMicroseconds(30);
        if (digitalRead(_pin) == HIGH)
        {
            data |= (1 << (7 - i));
            while (digitalRead(_pin) == HIGH)
                ;
        }
    }
    return data;
}

// Start the communication sequence with the DHT11 sensor
void SPMon_DHT11_Sensor_Library::DHT11_StartSequence()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    delay(18);
    digitalWrite(_pin, HIGH);
    delayMicroseconds(40);
    pinMode(_pin, INPUT);
}

// MAX6675 sensor library class
class SPMon_MAX6675_THCPL_Sensor_Library
{
public:
    SPMon_MAX6675_THCPL_Sensor_Library(uint8_t SCLK, uint8_t CS, uint8_t MISO);
    void MAX6675_GetRawData(SensorRawValues *sensorRawValues);
    void MAX6675_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);

private:
    uint8_t sclk, cs, miso;
    byte MAX6675_GetRawDataSequence();
    byte SPI_ReadRawFrame();
};

// Constructor
SPMon_MAX6675_THCPL_Sensor_Library::SPMon_MAX6675_THCPL_Sensor_Library(uint8_t SCLK, uint8_t CS, uint8_t MISO)
    : sclk(SCLK), cs(CS), miso(MISO)
{
    pinMode(cs, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(miso, INPUT);
    digitalWrite(cs, HIGH);
}

// Read raw data sequence from the MAX6675 sensor
byte SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetRawDataSequence()
{
    uint16_t SPIrawValue;
    digitalWrite(cs, LOW);
    delayMicroseconds(10);
    SPIrawValue = SPI_ReadRawFrame();
    SPIrawValue <<= 8;
    SPIrawValue |= SPI_ReadRawFrame();
    digitalWrite(cs, HIGH);

    if (SPIrawValue & 0x4)
    {
        Serial.println("Error: Thermocouple not attached!");
    }

    SPIrawValue >>= 3;
    return SPIrawValue;
}

// Read raw data from the MAX6675 sensor
void SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetRawData(SensorRawValues *sensorRawValues)
{
    uint16_t SPIrawData = MAX6675_GetRawDataSequence();
    sensorRawValues->thCplRawData = SPIrawData;
}

// Convert raw data to temperature
void SPMon_MAX6675_THCPL_Sensor_Library::MAX6675_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    uint16_t SPI_RawVal = sensorRawValues->thCplRawData;
    float TempVal = SPI_RawVal * 0.25;
    sensorConvertedValues->thCplConvData = TempVal;
}

// Read raw frame from the MAX6675 sensor
byte SPMon_MAX6675_THCPL_Sensor_Library::SPI_ReadRawFrame()
{
    byte spi_data_frame = 0;
    for (int8_t i = 7; i >= 0; i--)
    {
        digitalWrite(sclk, LOW);
        delayMicroseconds(50);
        if (digitalRead(miso))
        {
            spi_data_frame |= (1 << i);
        }
        digitalWrite(sclk, HIGH);
        delayMicroseconds(50);
    }
    return spi_data_frame;
}

// LM35 sensor library class
class SPMon_LM35_Sensor_Library
{
public:
    void LM35_GetRawData(SensorRawValues *sensorRawValues);
    void LM35_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues);
    void LM35_Calib(uint8_t sensorPin, uint8_t adcResolution, uint8_t adcAttenuation);
};

// Read raw data from the LM35 sensor
void SPMon_LM35_Sensor_Library::LM35_GetRawData(SensorRawValues *sensorRawValues)
{
    uint16_t adcData = 0;
#if LM35_OVERSAMPLING == TRUE
    for (uint8_t i = 0; i < LM35_MAX_SAMPLES; i++)
    {
        adcData += analogRead(LM35_PIN);
        delay(100);
    }
    adcData = adcData / LM35_MAX_SAMPLES;
#else
    adcData = analogRead(LM35_PIN);
#endif
    sensorRawValues->RawAdc_TempVal_LM35 = adcData;
}

// Convert raw data to temperature
void SPMon_LM35_Sensor_Library::LM35_GetTemp(SensorRawValues *sensorRawValues, SensorConvertedValues *sensorConvertedValues)
{
    float miliVolts_adcConv = (sensorRawValues->RawAdc_TempVal_LM35 * 3.3) / 4095;
    float miliVolts = analogReadMilliVolts(LM35_PIN);
    if ((miliVolts_adcConv + miliVolts) / 2 > 0 && (miliVolts_adcConv + miliVolts) / 2 < 3300)
    {
        float sampleValue = (miliVolts / 10.0) - 0;
        uint8_t avg_val[10];
        float avg_temp = 0;
        for (uint8_t i = 0; i < 10; i++)
        {
            avg_val[i] = sampleValue;
            avg_temp += avg_val[i];
        }
        avg_temp = avg_temp / 10;
        if (avg_temp > 150 || avg_temp < -55)
        {
        }
        else
        {
            sensorConvertedValues->ConValTempLM35 = avg_temp;
        }
    }
}

// Calibrate the LM35 sensor
void SPMon_LM35_Sensor_Library::LM35_Calib(uint8_t sensorPin, uint8_t adcResolution, uint8_t adcAttenuation)
{
    Serial.println("LM35 Calibration started");
    pinMode(sensorPin, INPUT);
    analogReadResolution(adcResolution);
    analogSetPinAttenuation(sensorPin, static_cast<adc_attenuation_t>(adcAttenuation));
    Serial.println("LM35 Calibration ended");
}

// DSM501A sensor library class
class SPMon_DSM501A_Sensor_Library
{
public:
    SPMon_DSM501A_Sensor_Library(uint8_t pinPM10, uint8_t pinPM25);
    void DSM501A_GetData(SensorRawValues *sensorRawValues);

private:
    uint8_t pinPM10, pinPM25;
    unsigned long duration;
    unsigned long starttime;
    unsigned long sampletime_ms;
    unsigned long lowpulseoccupancyPM25;
    unsigned long lowpulseoccupancyPM10;
    float ratioPM25;
    float ratioPM10;
    long concentrationPM25;
    long concentrationPM10;
};

// Constructor
SPMon_DSM501A_Sensor_Library::SPMon_DSM501A_Sensor_Library(uint8_t pinPM10, uint8_t pinPM25)
    : pinPM10(pinPM10), pinPM25(pinPM25), sampletime_ms(30000), lowpulseoccupancyPM25(0), lowpulseoccupancyPM10(0)
{
    pinMode(pinPM10, INPUT);
    pinMode(pinPM25, INPUT);
    starttime = millis();
}

// Read data from the DSM501A sensor
void SPMon_DSM501A_Sensor_Library::DSM501A_GetData(SensorRawValues *sensorRawValues)
{
    duration = pulseIn(pinPM25, LOW);
    lowpulseoccupancyPM25 += duration;
    duration = pulseIn(pinPM10, LOW);
    lowpulseoccupancyPM10 += duration;

    if ((millis() - starttime) >= sampletime_ms)
    {
        ratioPM25 = lowpulseoccupancyPM25 / (sampletime_ms * 10.0);
        concentrationPM25 = 1.1 * pow(ratioPM25, 3) - 3.8 * pow(ratioPM25, 2) + 520 * ratioPM25 + 0.62;
        sensorRawValues->concentrationPM25 = concentrationPM25;

        ratioPM10 = lowpulseoccupancyPM10 / (sampletime_ms * 10.0);
        concentrationPM10 = 1.1 * pow(ratioPM10, 3) - 3.8 * pow(ratioPM10, 2) + 520 * ratioPM10 + 0.62;
        sensorRawValues->concentrationPM10 = concentrationPM10;

        lowpulseoccupancyPM25 = 0;
        lowpulseoccupancyPM10 = 0;
        starttime = millis();
    }
}

// Global variables
SensorRawValues RawValues;
SensorConvertedValues ConvertedValues;
SPMon_BH1750_Sensor_Library bh1750(BH1750_ADDRESS, BH1750_MODE, BH1750_NR_OF_BYTES);
SPMon_DHT11_Sensor_Library dht11(DHT11_PIN);
SPMon_MAX6675_THCPL_Sensor_Library max6675(MAX6675_SCLK, MAX6675_CS, MAX6675_MISO);
SPMon_LM35_Sensor_Library lm35;
SPMon_DSM501A_Sensor_Library dsm501a(DUST_SENSOR_DIGITAL_PIN_PM10, DUST_SENSOR_DIGITAL_PIN_PM25);
INA3221 ina_0(INA3221_ADDR40_GND);

// Variables for the DSM501A sensor
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancyPM25 = 0;
unsigned long lowpulseoccupancyPM10 = 0;
float ratioPM25 = 0;
float ratioPM10 = 0;
long concentrationPM25 = 0;
long concentrationPM10 = 0;

/* INA3321 variables */
float cch1;
float cch2;
float cch3;
float vch1;
float vch2;
float vch3;

void current_measure_init()
{
    ina_0.begin(&Wire);
    ina_0.reset();
    ina_0.setShuntRes(100, 100, 100);
}

void measure_current()
{
    cch1 = ina_0.getCurrent(INA3221_CH1) * 1000;
    cch2 = ina_0.getCurrent(INA3221_CH2) * 1000;
    cch3 = ina_0.getCurrent(INA3221_CH3) * 1000;
    vch1 = ina_0.getVoltage(INA3221_CH1);
    vch2 = ina_0.getVoltage(INA3221_CH2);
    vch3 = ina_0.getVoltage(INA3221_CH3);
}

void setup()
{
    Serial.begin(115200);
    bh1750.BH1750_Init();
    lm35.LM35_Calib(LM35_PIN, 12, 3);

    // Initialize the DSM501A sensor
    pinMode(DUST_SENSOR_DIGITAL_PIN_PM10, INPUT);
    pinMode(DUST_SENSOR_DIGITAL_PIN_PM25, INPUT);
    starttime = millis();

    current_measure_init();

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected");

    configTime(7200, 0, "pool.ntp.org");
    Serial.println("Time synchronized!");
}

void loop()
{
    dsm501a.DSM501A_GetData(&RawValues);

    bh1750.BH1750_GetRawData(&RawValues);
    bh1750.BH1750_GetConvData(&RawValues, &ConvertedValues);

    dht11.DHT11_GetRawData(&RawValues);
    dht11.DHT11_GetConvData(&RawValues, &ConvertedValues);

    max6675.MAX6675_GetRawData(&RawValues);
    max6675.MAX6675_GetTemp(&RawValues, &ConvertedValues);

    lm35.LM35_GetRawData(&RawValues);
    lm35.LM35_GetTemp(&RawValues, &ConvertedValues);

    measure_current();

    Serial.println("Current measurement Ch1: " + String(cch1) + " mA");
    Serial.println("Current measurement Ch2: " + String(cch2) + " mA");
    Serial.println("Current measurement Ch3: " + String(cch3) + " mA");
    Serial.println("Voltage measurement Ch1: " + String(vch1) + " V");
    Serial.println("Voltage measurement Ch2: " + String(vch2) + " V");
    Serial.println("Voltage measurement Ch3: " + String(vch3) + " V");

    Serial.print("Raw Lux Value: ");
    Serial.println(RawValues.RawAdc_LuxVal);
    Serial.print("Converted Lux Value: ");
    Serial.println(ConvertedValues.ConValLux);

    Serial.print("DHT11 Raw Data: ");
    for (int i = 0; i < 5; i++)
    {
        Serial.print(RawValues.dhtRawData[i]);
        Serial.print(" ");
    }
    Serial.println();

    Serial.print("DHT11 Humidity: ");
    Serial.println(ConvertedValues.ConValDHT[0]);
    Serial.print("DHT11 Temperature: ");
    Serial.println(ConvertedValues.ConValDHT[1]);

    Serial.print("MAX6675 Raw Data: ");
    Serial.println(RawValues.thCplRawData);
    Serial.print("MAX6675 Temperature: ");
    Serial.println(ConvertedValues.thCplConvData);

    Serial.print("[DSM501A] PM2.5 Concentration = ");
    Serial.print(RawValues.concentrationPM25);
    Serial.print(" pcs/0.01cf");

    Serial.print(" | PM10 Concentration = ");
    Serial.print(RawValues.concentrationPM10);
    Serial.println(" pcs/0.01cf");

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;

        // Build URL 
        String url = googleScriptURL +
                     "?bh1750_lux=" + String(ConvertedValues.ConValLux) +
                     "&dht11_hum=" + String(ConvertedValues.ConValDHT[0]) +
                     "&dht11_temp=" + String(ConvertedValues.ConValDHT[1]) +
                     "&max6675_temp=" + String(ConvertedValues.thCplConvData) +
                     "&lm35_temp=" + String(ConvertedValues.ConValTempLM35) +
                     "&pm25_conc=" + String(RawValues.concentrationPM25) +
                     "&pm10_conc=" + String(RawValues.concentrationPM10) +
                     "&cch1=" + String(cch1) +
                     "&cch2=" + String(cch2) +
                     "&cch3=" + String(cch3) +
                     "&vch1=" + String(vch1) +
                     "&vch2=" + String(vch2) +
                     "&vch3=" + String(vch3);

        http.begin(url);                   //  GET init 
        int httpResponseCode = http.GET(); //  GET send

        if (httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println("HTTP Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        }
        else
        {
            Serial.println("Error on sending GET: " + String(httpResponseCode));
        }

        http.end(); // Close connection
    }
    else
    {
        Serial.println("WiFi not connected");
    }

    delay(2000); // Delay to avoid flooding the serial output
}
