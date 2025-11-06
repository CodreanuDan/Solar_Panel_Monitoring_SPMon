#include <SoftwareSerial.h>

// Define RS485 pins
const int RE_PIN = 8; 
const int DE_PIN = 9;
int k=0;

// SoftwareSerial for RS485 communication
SoftwareSerial rs485(10, 11); 

int interrogationWord = 87; 

void setup() {
  
  rs485.begin(9600);
  Serial.begin (9600);
  pinMode(RE_PIN, OUTPUT);
  pinMode(DE_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, HIGH); 
}

void loop() {
  // Enable RS485 transmit
  digitalWrite(DE_PIN, HIGH);
  delay(20); 

  // Send interrogation word
  rs485.write(interrogationWord); 
  delay(20); // Allow time for transmission
    if (k==0) 
        {digitalWrite(LED_BUILTIN, LOW);}
      else 
        {digitalWrite(LED_BUILTIN, HIGH);}
    if (k==0) 
        {k=1;}
      else 
        {k=0;}
  // Disable RS485 transmit, activate RS485 receive
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, LOW);
  delay(10);
    while (rs485.available() == 0);
  {  }
    Serial.print("t=");
    Serial.println(rs485.read());

    while (rs485.available() == 0);
  {  }
    Serial.print("h=");
    Serial.println(rs485.read());

  digitalWrite(DE_PIN, HIGH); //activate transmitt
  digitalWrite(RE_PIN, HIGH);

  
/*
  // Wait for response from slave
  while (rs485.available() == 0) { 
    // Wait for data to arrive
  }

  // Read data from slave
  float receivedData = rs485.parseFloat(); 
  Serial.print("Received from slave: ");
  Serial.println(receivedData); 
*/
  // Wait for a period before the next interrogation
  delay(10000); // 60 seconds (adjust as needed)
}
