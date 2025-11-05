/*
 * BLE Server test ESP32 - Send a dummy payload from the sensors to the to client
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

/* Dummy payload */
typedef struct __attribute__((packed)){
  uint16_t cnt = 0;
  float tempDHT = 22.52;
  float humDHT = 50.73;
  float tempDS18B20 = 20.14;
  float tempTHR = 25.67;
  uint16_t luxBH1750 = 750;
  uint16_t AS7341_spectre[11] = {34, 56, 79, 67, 90, 123, 670, 897, 89, 23, 67};
}SensorData;

/* BLE Server name */
#define BLE_SERVER_NAME "BLE_SERVER_ESP32"

/* Timer variables */
unsigned long lastTime = 0;
unsigned long timerInterval = 10000;

bool deviceConnected = false;

/* BLE Servive UUID */
#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"

/* Global BLE objects */
BLEServer *pServer = nullptr;
/* Define BLE Characteristic and Descriptor*/
BLECharacteristic testCharacteristic("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor testDescriptor(BLEUUID((uint16_t)0x2902));

/* Setup callbacks onConnect and onDisconnect */
class testServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer) override
    {
        deviceConnected = true;
        Serial.println(">>> Client connected!");
    }

    void onDisconnect(BLEServer* pServer) override
    { 
        deviceConnected = false;
        Serial.println(">>> Client disconnected!");
        /* Restart advertising */  
        BLEDevice::startAdvertising();
        Serial.println(">>> Restarted advertising...");
    }

};

void connect()
{
    /* Create the BLE device */
    BLEDevice::init(BLE_SERVER_NAME);

    /* Create the BLE server */
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new testServerCallbacks());

    /* Create the BLE Service */
    BLEService *testService = pServer->createService(SERVICE_UUID);

    /* Create BLE Characteristic and Descriptor*/
    testService->addCharacteristic(&testCharacteristic);
    testDescriptor.setValue("BLE dummy payload");
    testCharacteristic.addDescriptor(&testDescriptor);

    /* Start the device */
    testService->start();

    /* Strat advertising */
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println(">>> Waiting a client connection to notify...");
}


void setup()
{
    Serial.begin(115200);
    connect();
}

void loop()
{   
    /* Dummy payload */
    static SensorData payload;

    if (deviceConnected)
    {
        if ((millis() - lastTime) > timerInterval)
        {
            testCharacteristic.setValue((uint8_t*)&payload, sizeof(SensorData));
            testCharacteristic.notify();

            Serial.println(">>> Payload sent:");
            Serial.println(sizeof(SensorData));

            payload.cnt++;
            lastTime = millis();
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}