//Programul masoara componentele spectrale cu AS7341 si temperatura cu DS18B20, extrage timestamp din 
//NTP si trimite datele in cloud

#include <DS18B20.h>
#include <Adafruit_AS7341.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <time.h>

Adafruit_AS7341 as7341;
DS18B20 ds(23);

// Configurări Wi-Fi
const char* ssid = "SMM";        // Numele rețelei Wi-Fi
const char* password = "masurari1";      // Parola Wi-Fi
int violet, alb, albver, ver, galver, gal, portros, ros, nir, clear;
float temp3;

// URL-ul scriptului Google Apps Script
const char* scriptURL = "https://script.google.com/macros/s/AKfycbwyVrY21hZgL94utxeesytRDGT88m8uZhJnqRm7-G9jNEvvvGnzlfPfPbv00EI8Xzhe/exec";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

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

  if (!as7341.begin()){
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_1X);
}

void loop() {

  if (WiFi.status() == WL_CONNECTED) {
  uint16_t readings[12];
  if (!as7341.readAllChannels(readings)){
    Serial.println("Error reading all channels!");
    return;
  }
  ds.doConversion();
  violet = readings[0];
  alb = readings[1];
  albver = readings[2];
  ver = readings[3];
  galver = readings[6];
  gal = readings[7];
  portros = readings[8];
  ros = readings[9];
  nir = readings[11];
  clear = readings[10];
  temp3 = ds.getTempC();

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

  Serial.printf("Data: %s\n", date);
  Serial.printf("Ora: %s\n", time);
  Serial.printf("Violet415nm : ");
  Serial.println(violet);
  Serial.print("Albastru445nm: ");
  Serial.println(alb);
  Serial.print("Albastru-verde480nm : ");
  Serial.println(albver);
  Serial.print("Verde515nm : ");
  Serial.println(ver);
  Serial.print("Galben-verde 555nm : ");
  Serial.println(galver);
  Serial.print("Galben590nm : ");
  Serial.println(gal);
  Serial.print("Portocaliu-rosu630nm : ");
  Serial.println(portros);
  Serial.print("Rosu680nm : ");
  Serial.println(ros);
  Serial.print("NIR910nm : ");
  Serial.println(nir);
  Serial.print("Clear : ");
  Serial.println(clear);
  Serial.print("Temperatura: ");
  Serial.print(temp3);
  Serial.println();
  
// Construiește payload-ul JSON
    String jsonPayload = "{";
    jsonPayload += "\"date\":\"" + String(date) + "\",";
    jsonPayload += "\"time\":\"" + String(time) + "\",";
    jsonPayload += "\"violet\":" + String(violet) + ",";
    jsonPayload += "\"alb\":" + String(alb) + ",";
    jsonPayload += "\"albver\":" + String(albver) + ",";
    jsonPayload += "\"ver\":" + String(ver) + ",";
    jsonPayload += "\"galver\":" + String(galver) + ",";
    jsonPayload += "\"gal\":" + String(gal) + ",";
    jsonPayload += "\"portros\":" + String(portros) + ",";
    jsonPayload += "\"ros\":" + String(ros) + ",";
    jsonPayload += "\"nir\":" + String(nir) + ",";
    jsonPayload += "\"clear\":" + String(clear) + ",";
    jsonPayload += "\"temp3\":" + String(temp3);
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
  }  
    else {
    Serial.println("Wi-Fi disconnected!");
    WiFi.begin(ssid, password);
    }

  delay(60000);

}