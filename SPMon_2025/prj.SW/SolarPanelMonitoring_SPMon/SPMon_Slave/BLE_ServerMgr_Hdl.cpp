/*
 * BLE Server for ESP32 - Send the payload from the sensors to the to client
 */

#include "BLE_ServerMgr_Hdl.h"

void BLE_ServerMgrHdl::createServer()
{
    /* Create the BLE device */
    BLEDevice::init(BLE_SERVER_NAME);

    /* Create the BLE server */
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    /* Create the BLE Service */
    BLEService *pService = pServer->createService(SERVICE_UUID);

    /* Create BLE Characteristic*/
    pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY);

    /* Create BLE Descriptor*/
    pCharacteristic->addDescriptor(new BLE2902());

    /* Start the device */
    pService->start();

    /* Strat advertising */
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println(">>> Waiting a client connection to notify...");

}

void BLE_ServerMgrHdl::sendPayload(const uint8_t* data, size_t length) 
{
    if (deviceConnected && pCharacteristic) 
    {
        pCharacteristic->setValue((uint8_t*)data, length);
        pCharacteristic->notify();
        Serial.printf(">>> Payload of %u bytes sent via BLE notify.\n", (unsigned int)length);
    }
}

void BLE_ServerMgrHdl::ServerCallbacks::onConnect(BLEServer* pServer) 
{
    deviceConnected = true;
    Serial.println(">>> Client connected!");
}
vf
void BLE_ServerMgrHdl::ServerCallbacks::onDisconnect(BLEServer* pServer) 
{
    deviceConnected = false;
    Serial.println(">>> Client disconnected!");
    BLEDevice::startAdvertising();
    Serial.println(">>> Advertising restarted...");
}