#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <INA3221.h>
#include <time.h>
#include <SoftwareSerial.h>

float cch1;
float cch2;
float cch3;
float vch1;
float vch2;
float vch3;


// Set I2C address to 0x41 (A0 pin -> VCC)
INA3221 ina_0(INA3221_ADDR40_GND);

// Define RS485 pins
const int RE_PIN = 26; 
const int DE_PIN = 25;
int interrogationWord = 87;

// SoftwareSerial for RS485 communication
SoftwareSerial rs485(35, 32); 

// Configurări Wi-Fi
const char* ssid = "SMM";        // Numele rețelei Wi-Fi
const char* password = "masurari1";      // Parola Wi-Fi

// URL-ul scriptului Google Apps Script
const char* scriptURL = "https://script.google.com/macros/s/AKfycbzPXOJUHjVa08vXkIq-VmLh6A-E9OAMflTlPaREZtD5AcI6ue01WtyRkjtcU9g6kziO/exec";

void current_measure_init() {
    ina_0.begin(&Wire);
    ina_0.reset();
    ina_0.setShuntRes(100, 100, 100);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  current_measure_init();
  rs485.begin(9600);

  pinMode(RE_PIN, OUTPUT);
  pinMode(DE_PIN, OUTPUT);
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, HIGH);

   // Conectare Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Configurare sincronizare timp
  configTime(7200, 0, "pool.ntp.org");
  Serial.println("Time synchronized!");
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    cch1=ina_0.getCurrent(INA3221_CH1) * 1000;
    cch2=ina_0.getCurrent(INA3221_CH2) * 1000;
    cch3=ina_0.getCurrent(INA3221_CH3) * 1000;
    vch1=ina_0.getVoltage(INA3221_CH1);
    vch2=ina_0.getVoltage(INA3221_CH2);
    vch3=ina_0.getVoltage(INA3221_CH3);
  // Enable RS485 transmit
  digitalWrite(DE_PIN, HIGH);
  delay(20); 

  // Send interrogation word
  rs485.write(interrogationWord); 
  delay(20); // Allow time for transmission

   // Disable RS485 transmit, activate RS485 receive
  digitalWrite(DE_PIN, LOW);
  digitalWrite(RE_PIN, LOW);
  delay(10);

while (rs485.available() == 0);
{}
float temp1=rs485.parseFloat();
while (rs485.available() == 0);
{}
float humid=rs485.parseFloat();
while (rs485.available() == 0);
{}
float ilum1=rs485.parseFloat();
float ilum=3.6*ilum1;
while (rs485.available() == 0);
{}
float temp2=rs485.parseFloat();
while (rs485.available() == 0);
{}
float praf=rs485.parseFloat();

  digitalWrite(DE_PIN, LOW); //activate transmitt
  digitalWrite(RE_PIN, HIGH);

// Enable RS485 transmit
  digitalWrite(DE_PIN, HIGH);
  delay(20); 

    // Obține timpul curent
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      delay(1000);
      return;
    }

      // Construiește data și ora
    char date[11]; // Buffer pentru data: YYYY-MM-DD
    char time[9];  // Buffer pentru ora: HH:MM:SS
    strftime(date, sizeof(date), "%Y-%m-%d", &timeinfo);
    strftime(time, sizeof(time), "%H:%M:%S", &timeinfo);

  //Afiseaza data, ora si valorile citite
  Serial.printf("Data: %s\n", date);
  Serial.printf("Ora: %s\n", time);
  Serial.printf("Temperatura DHT22: %3.2f grade\n", temp1);
  Serial.printf("Umiditatea: %3.2f RH\n", humid);
  Serial.printf("Iluminarea: %3.2f lux\n", ilum);
  Serial.printf("Temperatura NTC: %3.2f grade\n", temp2);
  Serial.printf("Praf: %3.0f\n", praf);
  Serial.printf("Curent canal1 %3.1f mA\n", cch1);
  Serial.printf("Curent canal2 %3.1f mA\n", cch2);
  Serial.printf("Curent canal3 %3.1f mA\n", cch3);
  Serial.printf("Tensiune canal1 %3.1f V\n", vch1);
  Serial.printf("Tensiune canal2 %3.1f V\n", vch2);
  Serial.printf("Tensiune canal3 %3.1f V\n", vch3);

  
// Construiește payload-ul JSON
    String jsonPayload = "{";
    jsonPayload += "\"date\":\"" + String(date) + "\",";
    jsonPayload += "\"time\":\"" + String(time) + "\",";
    jsonPayload += "\"temp1\":" + String(temp1) + ",";
    jsonPayload += "\"humid\":" + String(humid) + ",";
    jsonPayload += "\"ilum\":" + String(ilum) + ",";
    jsonPayload += "\"temp2\":" + String(temp2) + ",";
    jsonPayload += "\"praf\":" + String(praf) + ",";
    jsonPayload += "\"cch1\":" + String(cch1) + ",";
    jsonPayload += "\"cch2\":" + String(cch2) + ",";
    jsonPayload += "\"cch3\":" + String(cch3) + ",";
    jsonPayload += "\"vch1\":" + String(vch1) + ",";
    jsonPayload += "\"vch2\":" + String(vch2) + ",";
    jsonPayload += "\"vch3\":" + String(vch3);
    jsonPayload += "}";

Serial.println(jsonPayload);
// Trimite datele către Google Sheets
    HTTPClient http;
    http.begin(scriptURL);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonPayload);

// Afișează rezultatul
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully!");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }

http.end();
  } else {
    Serial.println("Wi-Fi disconnected!");
    WiFi.begin(ssid, password);
  }

delay(60000);
}
