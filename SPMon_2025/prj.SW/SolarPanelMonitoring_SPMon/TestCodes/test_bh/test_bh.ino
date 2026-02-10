#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  if (!lightMeter.begin()) {
    Serial.println("BH1750 not found");
    while (1);
  }

  Serial.println("BH1750 test started");
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.printf("Light: %.2f lux\n", lux);
  delay(1000);
}
