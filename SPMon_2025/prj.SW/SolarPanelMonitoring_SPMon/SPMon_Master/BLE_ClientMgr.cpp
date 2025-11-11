#include "BLE_ClientMgr.h"
#include <Arduino.h>

/* Activate notify */
const uint8_t notificationOn[] = { 0x1, 0x0 };

void BLE_ClientMgr::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
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

void BLE_ClientMgr::testNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                        uint8_t* pData,
                                        size_t length,
                                        bool isNotify)
{
    if (length == sizeof(SensorPayload))
    {
        memcpy(&payload, pData, sizeof(SensorPayload));
        Serial.println(">>> PAYLOAD RECEIVED:");

        Serial.printf("CNT: %d | DHT_T: %.2f | DHT_H: %.2f | DS18B20: %.2f | THR: %.2f | PRESSURE: %.2f | LUX: %d\n",
                      payload.cnt,
                      payload.tempDHT,
                      payload.humDHT,
                      payload.tempDS18B20,
                      payload.tempTHR,
                      payload.pressure, 
                      payload.lux);

        Serial.print("Spectre: ");
        for (int i = 0; i < 10; i++) 
        {
            Serial.print(payload.spec[i]);
            if (i < 9) Serial.print(","); 
        }
        Serial.println();
        Serial.print(">>> Packet length: "); Serial.println(length);
    }
    else
    {
        Serial.print(">>> Wrong packet size! Expected ");
        Serial.print(sizeof(SensorPayload));
        Serial.print(" bytes, but received ");
        Serial.print(length);
        Serial.println(" bytes.");
    }
}

bool BLE_ClientMgr::connectToServer(BLEAddress pAddress)
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
    testCharacteristic->registerForNotify(testNotifyCallback);
    return true;
}

void BLE_ClientMgr::initBLEService()
{
    /* Init BLE device */
    BLEDevice::init("");
    BLEDevice::setMTU(517);
    /*
      * Retrieve a Scanner and set the callback we want to use to be informed when we
      * have detected a new device. Specify that we want active scanning and start the
      * scan to run for 30 seconds.
      */
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

void BLE_ClientMgr::checkCharacterisitc()
{
    if (connected)
    {
        BLEClient* pClient = testCharacteristic ?
            testCharacteristic->getRemoteService()->getClient() : nullptr;
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

void BLE_ClientMgr::restartScan()
{
    if (!connected && !doConnect)
    {
        Serial.println(">>> Not connected. Starting scan...");
        BLEScan* pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
        pBLEScan->setActiveScan(true);
        pBLEScan->start(5);  // scan de 5 secunde
    }
}

void BLE_ClientMgr::establishCommunication()
{
    if (doConnect && pServerAddress != nullptr)
    {
        if (connectToServer(*pServerAddress))
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

void BLE_ClientMgr::init()
{
    Serial.println(">>> Starting Arduino BLE Client application...");
    initBLEService();
}

void BLE_ClientMgr::communicationManagerMainFunction()
{
    /* 1. Check that the client is connected and charactreisitc is valid */
    checkCharacterisitc();
    /* 2. If connection is lost, restart scan */
    restartScan();
    /* 3. If the server is found try to connect */
    establishCommunication();
}