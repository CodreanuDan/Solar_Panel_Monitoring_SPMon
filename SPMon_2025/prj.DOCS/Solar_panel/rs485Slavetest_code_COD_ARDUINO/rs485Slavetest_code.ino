#include <SoftwareSerial.h>
#include <DHT.h>
#include <Wire.h>
#include <BH1750.h>



// Define RS485 pins
const int nRE = 8; 
const int DI = 9;
int k = 0;
// Define DHT22 sensor pin
#define DHTPIN 7 
#define DHTTYPE DHT22   
SoftwareSerial mySerial(10, 11);
BH1750 lightMeter; 

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);



void setup() {
  mySerial.begin(9600);
  // Serial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  dht.begin();


  pinMode(nRE, OUTPUT);
  pinMode(DI, OUTPUT);

  digitalWrite(nRE, LOW); //receive active
  digitalWrite(DI, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(A0, INPUT); // pinul A0 intrare
  pinMode(A1, INPUT); // pinul A1 intrare
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
      float lux = lightMeter.readLightLevel();
      float volt1 = analogRead(A0)*5.000/1024;  //Read Analog value of NTC1
      //float volt2 = analogRead(A0)*5.000/1024;  //Read Analog value of NTC2
      float voltref=analogRead(A1)*5.000/1024; // Read NTC Supply voltage
      float Rntc1 = (volt1*19.96)/(voltref*1.9916-volt1); // calculate NTC22k resistance from voltage
      float temp1 = 4200/(log(Rntc1/(76.8*exp(-4200/273.75))))-273.15; // NTC22k calculate temperature from voltage
      float temp1c=0.9593*temp1+0.9555; // curve calibration aftwer experimental values
      //float Rntc2 = (volt2*19.96)/(voltref*1.9916-volt2); // calculate NTC22k resistance from voltage
      //float temp2 = 4200/(log(Rntc2/(76.8*exp(-4200/273.75))))-273.15; //  // NTC22k calculate temperature from voltage
      //float temp2c=0.9593*temp2+0.9555; // curve calibration aftwer experimental values

        digitalWrite(nRE, HIGH);
        digitalWrite(DI, HIGH); //transmit active
        delay(20);
      mySerial.println(t,2);
      delay(100);
      mySerial.println(h,2);
      delay(100);
      mySerial.println(lux,2);
      delay(100);
      mySerial.println(temp1c,2);
      delay(100);
      //mySerial.write(temp2c); 
      //delay(20);

      digitalWrite(nRE, LOW); // receive active
      digitalWrite(DI, LOW);       

    }
    // else {}
}
