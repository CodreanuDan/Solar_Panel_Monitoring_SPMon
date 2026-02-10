#include <Wire.h>
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  if (!as7341.begin()) {
    Serial.println("AS7341 not found");
    while (1);
  }

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_1X);

  Serial.println("AS7341 test started");
}

void loop() {
  uint16_t readings[10];

  if (as7341.readAllChannels(readings)) {
    for (int i = 0; i < 10; i++) {
      Serial.printf("CH[%d]: %u  ", i, readings[i]);
    }
    Serial.println();
  } else {
    Serial.println("Read failed");
  }

  delay(2000);
}
