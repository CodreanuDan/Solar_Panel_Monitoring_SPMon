#include "BLE_ClientMgr.h"
#include <esp_bt.h>

BLEClient* BLE_ClientMgr::pCurrentClient = nullptr;
SensorPayload BLE_ClientMgr::payload;
BLERemoteCharacteristic* BLE_ClientMgr::testCharacteristic = nullptr;
BLEAddress* BLE_ClientMgr::pServerAddress = nullptr;
boolean BLE_ClientMgr::doConnect = false;
boolean BLE_ClientMgr::connected = false;
boolean BLE_ClientMgr::newDataReceived = false; 

const BLEUUID BLE_ClientMgr::serverServiceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
const BLEUUID BLE_ClientMgr::testCharacteristicUUID("cba1d466-344c-4be3-ab3f-189f80dd7518");

void printHexDump(const uint8_t* data, size_t length) 
{
    Serial.print(">>> BLE Raw Payload (hex): ");
    for (size_t i = 0; i < length; i++) 
    {
        if (data[i] < 0x10) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
        if ((i + 1) % 16 == 0) Serial.println();
    }
    if (length % 16 != 0) Serial.println();
}

void BLE_ClientMgr::init() 
{
    Serial.println(">>> BLE: Initializing controller...");
    BLEDevice::init("SPMon_Master_BLE"); 
    initBLEService();
    Serial.println(">>> BLE: Scan started.");
}

void BLE_ClientMgr::stopBLE() 
{
    Serial.println(">>> BLE: Disconnecting and stopping controller...");
    BLEScan* pBLEScan = BLEDevice::getScan();
    if (pBLEScan) pBLEScan->stop();
    if (pCurrentClient && pCurrentClient->isConnected()) 
    {
        pCurrentClient->disconnect();
    }
    delete pCurrentClient;
    pCurrentClient = nullptr;
    delete pServerAddress;
    pServerAddress = nullptr;
    testCharacteristic = nullptr;
    connected = false;
    doConnect = false;
    BLEDevice::deinit(); 
    delay(100);
    Serial.println(">>> BLE: Controller stopped (resources freed for WiFi).");
}


void BLE_ClientMgr::communicationManagerMainFunction()
{
    if (!connected && doConnect && pServerAddress != nullptr)
    {
        Serial.println(">>> BLE: Attempting connection...");
        connected = connectToServer(*pServerAddress);
        doConnect = false;
        Serial.println(connected ? ">>> BLE: Connected!" : ">>> BLE: Connection failed");
    }

    if (connected) checkCharacterisitc();
}

void BLE_ClientMgr::initBLEService()
{
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(160);
    pBLEScan->setWindow(80);
    pBLEScan->start(0, false); 
}

void BLE_ClientMgr::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    if (advertisedDevice.getName() == BLE_ClientMgr::BLE_SERVER_NAME)
    {
        Serial.println(">>> BLE: Found BLE_SERVER_ESP32! Stopping scan...");
        
        BLEDevice::getScan()->stop(); 

        Serial.print(">>> BLE: Address: ");
        Serial.println(advertisedDevice.getAddress().toString().c_str());

        if (pServerAddress) delete pServerAddress;
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        doConnect = true;
    }
}

bool BLE_ClientMgr::connectToServer(BLEAddress pAddress)
{
    if (pCurrentClient) 
    {
        if (pCurrentClient->isConnected()) pCurrentClient->disconnect();
        delete pCurrentClient;
        pCurrentClient = nullptr;
    }

    Serial.print(">>> BLE: Connecting to ");
    Serial.println(pAddress.toString().c_str());

    pCurrentClient = BLEDevice::createClient();
    if (!pCurrentClient->connect(pAddress, BLE_ADDR_TYPE_PUBLIC, 15000)) 
    { 
        Serial.println(">>> BLE: Connect failed");
        delete pCurrentClient; pCurrentClient = nullptr;
        return false;
    }
    
    Serial.println(">>> BLE: Requesting 517 byte MTU...");
    pCurrentClient->setMTU(517); 

    auto* pRemoteService = pCurrentClient->getService(serverServiceUUID);
    if (!pRemoteService) { Serial.println(">>> BLE: Service not found"); pCurrentClient->disconnect(); delete pCurrentClient; pCurrentClient = nullptr; return false; }

    testCharacteristic = pRemoteService->getCharacteristic(testCharacteristicUUID);
    if (!testCharacteristic || !testCharacteristic->canNotify()) 
    {
        Serial.println(">>> BLE: Char not found / no notify");
        pCurrentClient->disconnect(); delete pCurrentClient; pCurrentClient = nullptr;
        return false;
    }

    testCharacteristic->registerForNotify(testNotifyCallback);
    connected = true;
    Serial.println(">>> BLE: CONNECTED + NOTIFY ACTIVE! Waiting for payload...");
    return true;
}

=====
void BLE_ClientMgr::testNotifyCallback(BLERemoteCharacteristic*, uint8_t* pData, size_t length, bool)
{
    if (length == sizeof(SensorPayload)) 
    { 
        
        Serial.println("\n--- Payload COMPLETE (44B) received ---");
        printHexDump(pData, length); 
        memcpy(&payload, pData, sizeof(SensorPayload));
        Serial.printf(">>> PAYLOAD: CNT=%d | T=%.2f°C | H=%.1f%% | DS18=%.2f°C | THR=%.2f°C | PRESSURE=%.2f | LUX=%d\n",
                      payload.cnt, payload.tempDHT, payload.humDHT, payload.tempDS18B20, payload.tempTHR, payload.pressure, payload.lux);
        
        BLE_ClientMgr::newDataReceived = true; 

    } 
    else 
    {
        Serial.printf(">>> ERORR: Wrong length %d (expected %luB).\n", length, sizeof(SensorPayload));
    }
}

void BLE_ClientMgr::checkCharacterisitc()
{
    if (!testCharacteristic || !pCurrentClient || !pCurrentClient->isConnected()) 
    {
        Serial.println(">>> BLE: Connection lost during active phase. Stopping BLE controller.");
        stopBLE(); 
    }
}