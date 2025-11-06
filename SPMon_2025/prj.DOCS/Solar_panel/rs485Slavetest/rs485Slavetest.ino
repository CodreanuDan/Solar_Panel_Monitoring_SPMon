#include <SoftwareSerial.h>
#include <DHT.h>


// Define RS485 pins
const int nRE = 8; 
const int DI = 9;
int k = 0;
// Define DHT22 sensor pin
#define DHTPIN 2 
#define DHTTYPE DHT11   
SoftwareSerial mySerial(10, 11); 

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  mySerial.begin(9600);
  // Serial.begin(9600);

  pinMode(nRE, OUTPUT);
  pinMode(DI, OUTPUT);
  digitalWrite(nRE, LOW); //receive active
  digitalWrite(DI, LOW);
  dht.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
while (mySerial.available() == 0);
  {}

  if (mySerial.read()==87)  // received word is W
     {
          if (k==0) 
        {digitalWrite(LED_BUILTIN, LOW);}
      else 
        {digitalWrite(LED_BUILTIN, HIGH);}
    if (k==0) 
        {k=1;}
      else 
        {k=0;}
      float h = dht.readHumidity();
      // Read temperature as Celsius
      float t = dht.readTemperature();
        digitalWrite(nRE, HIGH);
        digitalWrite(DI, HIGH); //trasmit active
        delay(20);
      mySerial.write(t);
      delay(100);
      mySerial.write(h); 
      delay(20);
      digitalWrite(nRE, LOW); // receive active
      digitalWrite(DI, LOW);       

    }
    // else {}
}
