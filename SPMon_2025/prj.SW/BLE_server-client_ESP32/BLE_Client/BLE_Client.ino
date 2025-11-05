/*
 * BLE Client test ESP32 - Receive a dummy payload from the server and print it on client side
 */

#include "BLEDevice.h"

/* Expected format and size of the payload */
typedef struct __attribute__((packed)) {
    uint16_t cnt;
    float tempDHT;
    float humDHT;
    float tempDS18B20;
    float tempTHR;
    uint16_t lux;
    uint16_t spec[11];
} SensorPayload;

SensorPayload payload;

/* BLE Communictaion states */
typedef enum{
	BLE_NOT_CONNECTED = 0u,
	BLE_RESCAN,
	BLE_CONNECTED,
}BLE_States;

/* BLE Server name (the other ESP32 name running the server sketch) */
#define BLE_SERVER_NAME "BLE_SERVER_ESP32"

/* UUID's of the service, characteristic that we want to read*/
static BLEUUID serverServiceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");

/* Characteristic */
static BLEUUID testCharacteristicUUID("cba1d466-344c-4be3-ab3f-189f80dd7518");

/* Flags stating if should begin connecting and if the connection is up */
static boolean doConnect = false;
static boolean connected = false;

/* Address of the peripheral device. Address will be found during scanning */
static BLEAddress* pServerAddress;

/* Characteristics that we want to read */
static BLERemoteCharacteristic* testCharacteristic;

/* Activate notify */
const uint8_t notificationOn[] = { 0x1, 0x0 };
const uint8_t notificationOff[] = { 0x0, 0x0 };


/* Connect to the BLE Server that has the name, Service, and Characteristics */
static bool BLE_Manager_connectToServer(BLEAddress pAddress);
/* When the BLE Server sends a new reading with the notify property */
static void BLE_Manager_testNotifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                uint8_t* pData,
                                size_t length,
                                bool isNotify);
/* Init BLE */								
static void BLE_Manager_initBLEService();

/* Check that the client is connected and charactreisitc is valid */
static void BLE_Manager_checkCharacterisitc();
/* If connection is lost, restart scan */
static void BLE_Manager_restartScan();
/* If the server is found try to connect */
static void BLE_Manager_establishCommunication();
/* BLE communication manager main function */
static void BLE_Manager_communicationManagerMainFunction();



/* Callback function that gets called, when another device's advertisement has been received */
class BLE_Manager_AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) 
    {
        /* Check if the name of the advertiser matches */
        if (advertisedDevice.getName() == BLE_SERVER_NAME) 
        {
            /* Scan can be stopped, we found what we are looking for */
            advertisedDevice.getScan()->stop();

            /* Address of advertiser is the one we need */
            pServerAddress = new BLEAddress(advertisedDevice.getAddress());

            /* Set indicator, stating that we are ready to connect */
            doConnect = true;
            Serial.println(">>> Device found. Connecting!");
        }
    }
};



void setup() 
{
    Serial.begin(115200);
    Serial.println(">>> Starting Arduino BLE Client application...");

    /* Init BLE device */
	BLE_Manager_initBLEService();
	
}

void loop() 
{
    /* 1. Check that the client is connected and charactreisitc is valid */


    /* 2. If connection is lost, restart scan */ 


    /* 3. If the server is found try to connect */ 


    delay(1000); 
}

/* Connect to the BLE Server that has the name, Service, and Characteristics */
static bool BLE_Manager_connectToServer(BLEAddress pAddress) 
{
    BLEClient* pClient = BLEDevice::createClient();

    /* Connect to the remote BLE Server */
    pClient->connect(pAddress);
    /* Set maximum packet size */
    pClient->setMTU(517);
    Serial.println(">>> Connected to server");

    /*Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(serverServiceUUID);
    if (pRemoteService == nullptr) 
    {
        Serial.print(">>> Failed to find our service UUID: ");
        Serial.println(serverServiceUUID.toString().c_str());
        return (false);
    }

    /* Obtain a reference to the characteristics in the service of the remote BLE server */
    testCharacteristic = pRemoteService->getCharacteristic(testCharacteristicUUID);
    if (testCharacteristic == nullptr) 
    {
        Serial.print(">>> Failed to find our characteristic UUID");
        return false;
    }
    Serial.println(">>> Found our characteristics");

    /* Assign callback functions for the Characteristics */
    testCharacteristic->registerForNotify(BLE_Manager_testNotifyCallback);
    return true;
}

/* When the BLE Server sends a new reading with the notify property */
static void BLE_Manager_testNotifyCallback( BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                uint8_t* pData,
                                size_t length,
                                bool isNotify) 
{
    if (length == sizeof(SensorPayload)) 
    {
        memcpy(&payload, pData, sizeof(SensorPayload));

        Serial.println(">>> PAYLOAD RECEIVED:");
        Serial.printf("CNT: %d | DHT_T: %.2f | DHT_H: %.2f | DS18B20: %.2f | THR: %.2f | LUX: %d\n",
                      payload.cnt, 
                      payload.tempDHT, 
                      payload.humDHT, 
                      payload.tempDS18B20,
                      payload.tempTHR, 
                      payload.lux);
        Serial.print("Spectre: ");
        for (int i = 0; i < 11; i++) 
        {
            Serial.print(payload.spec[i]);
            if (i < 10) Serial.print(",");
        }
        Serial.println();
        Serial.print(">>> Packet length: "); Serial.println(length);

    } 
    else 
    {
        Serial.println(">>> Wrong packet size!");
        Serial.println(length);
    }
}

/* Init BLE */								
static void BLE_Manager_initBLEService()
{
	/* Init BLE device */
    BLEDevice::init("");
    BLEDevice::setMTU(517);

    /* 
      * Retrieve a Scanner and set the callback we want to use to be informed when we
      * have detected a new device.  Specify that we want active scanning and start the
      * scan to run for 30 seconds. 
      */
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new BLE_Manager_AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

/* Check that the client is connected and charactreisitc is valid */
static void BLE_Manager_checkCharacterisitc()
{
	if (connected) 
    {
        BLEClient* pClient = testCharacteristic ? testCharacteristic->getRemoteService()->getClient() : nullptr;
        if (!pClient || !pClient->isConnected()) 
        {
            Serial.println(">>> Server disconnected, will reconnect...");
            connected = false;

            /* Delete old charactreisitc and address if connection is lost*/ 
            testCharacteristic = nullptr;
            delete pServerAddress;
            pServerAddress = nullptr;
        }
    }
}

/* If connection is lost, restart scan */
static void BLE_Manager_restartScan()
{
	if (!connected && !doConnect) 
    {
        Serial.println(">>> Not connected. Starting scan...");
        BLEScan* pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(new BLE_Manager_AdvertisedDeviceCallbacks());
        pBLEScan->setActiveScan(true);
        pBLEScan->start(5);  // scan de 5 secunde
    }
}

/* If the server is found try to connect */
static void BLE_Manager_establishCommunication()
{
	  if (doConnect && pServerAddress != nullptr) 
    {
        if (BLE_Manager_connectToServer(*pServerAddress)) 
        {
            Serial.println(">>> We are now connected to the BLE Server.");
            testCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
            connected = true;
        } 
        else 
        {
            Serial.println(">>> Failed to connect, will retry scanning...");
            connected = false;
            delete pServerAddress;
            pServerAddress = nullptr;
        }
        doConnect = false;
    }
}

/* BLE communication manager main function */
static void BLE_Manager_communicationManagerMainFunction()
{
	
}
