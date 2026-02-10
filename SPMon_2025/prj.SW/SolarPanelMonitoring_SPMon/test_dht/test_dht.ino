#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    dht.begin();
}

void loop()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (!isnan(humidity) && !isnan(temperature))
    {
        Serial.print(temperature);
        Serial.print(" ");
        Serial.println(humidity);
    }

    delay(2000);
}
