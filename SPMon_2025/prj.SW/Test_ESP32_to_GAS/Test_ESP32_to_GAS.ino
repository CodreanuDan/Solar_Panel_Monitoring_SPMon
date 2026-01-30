#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"

// ----------------------------
// CONFIG WiFi + Google Script
// ----------------------------
// #define WIFI_SSID      "SMM"
// #define WIFI_PASSWORD  "masurari1"

#define WIFI_SSID      "TP-Link_47D4"
#define WIFI_PASSWORD  "29393145"

#define SCRIPT_URL "https://script.google.com/macros/s/AKfycbwzeNJzdtdMVYKYHxe09Y0Hr9_8betnzkY9nK4Sjz2Sn1O5qQOfaJmLY_VGgSBW_1hZoQ/exec"

// ----------------------------
// NTP CONFIG (Romania)
// ----------------------------
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;       // GMT+2
const int   daylightOffset_sec = 3600;  // DST

// ----------------------------
// TIMING NON-BLOCKING
// ----------------------------
unsigned long lastPost = 0;
const unsigned long POST_INTERVAL = 10000;  // 10 secunde

// ----------------------------
// TIMESTAMP FUNCTION
// ----------------------------
String getTimestamp() 
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) 
  {
    return "Time Sync Error";
  }

  char timeString[20];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(timeString);
}

// =======================================================
//  SEND DATA TO GOOGLE SHEETS – NON BLOCKING POST JSON
// =======================================================
void sendToSheet() 
{
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.println("WiFi not connected - abort POST!");
    return;
  }

  StaticJsonDocument<512> doc;

  // Timestamp NTP
  doc["Timestamp"] = getTimestamp();

  // ----------------------------
  // DUMMY DATA
  // ----------------------------
  doc["BLE_CNT"] = 10;
  doc["DHT_Temp"] = 23.5;
  doc["DHT_Hum"] = 55.2;
  doc["DS_Temp"] = 21.9;
  doc["THR_Temp"] = 22.7;
  doc["Pressure"] = 1005.3;
  doc["LUX"] = 350;

  doc["Spec_Violet"] = 123;
  doc["Spec_White"] = 234;
  doc["Spec_White_Green"] = 212;
  doc["Spec_Green"] = 210;
  doc["Spec_Yellow_Green"] = 215;
  doc["Spec_Yellow"] = 198;
  doc["Spec_Orange_Red"] = 188;
  doc["Spec_Red"] = 200;
  doc["Spec_NIR"] = 190;
  doc["Spec_Clear"] = 205;

  doc["ICH1"] = 10;
  doc["UCH1"] = 20;
  doc["ICH2"] = 11;
  doc["UCH2"] = 21;
  doc["ICH3"] = 12;
  doc["UCH3"] = 22;

  // Serialize JSON
  String json;
  serializeJson(doc, json);

  // ----------------------------
  // SEND POST
  //-----------------------------
  HTTPClient http;
  http.begin(SCRIPT_URL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(json);

  Serial.print("POST Response: ");
  Serial.println(httpResponseCode);

  if (httpResponseCode > 0) 
  {
    Serial.println(http.getString());
  }

  http.end();
}

// =======================================================
// SETUP
// =======================================================
void setup() 
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("TEST 1");

  Serial.print("Connecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK!");

  // NTP sync
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.print("NTP Sync...");
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5000)) 
  {
    Serial.println(" FAILED");
  } 
  else 
  {
    Serial.println(" SUCCESS");
  }
}

// =======================================================
// LOOP – NON BLOCKING
// =======================================================
void loop() {
  unsigned long now = millis();


  // ----------------------------
  // NON BLOCKING POST TIMER
  // ----------------------------
  if (now - lastPost >= POST_INTERVAL) 
  {
    lastPost = now;
    sendToSheet();
  }
}
