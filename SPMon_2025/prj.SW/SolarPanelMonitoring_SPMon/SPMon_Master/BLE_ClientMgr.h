#ifndef BLE_CLIENT_MGR_H
#define BLE_CLIENT_MGR_H

#include "BLEDevice.h"
#include <BLEAdvertisedDevice.h>
#include "SensorPayload.h"

class BLE_ClientMgr
{
public:

    static bool isNewDataReceived();
    static void clearNewDataFlag();

    static void init(); 
    static void communicationManagerMainFunction(); 
    static void stopBLE(); 

    static const SensorPayload& getPayload() { return payload; }
    
    static boolean newDataReceived; 
    static boolean connectionLost;  /* NEW to counter BLE blockings*/

private:
    static BLEClient* pCurrentClient;

    static constexpr const char* BLE_SERVER_NAME = "BLE_SERVER_ESP32";

    static const BLEUUID serverServiceUUID;
    static const BLEUUID testCharacteristicUUID;

    static SensorPayload payload;
    static BLERemoteCharacteristic* testCharacteristic;
    static BLEAddress* pServerAddress;
    static boolean doConnect;
    static boolean connected;

    static bool connectToServer(BLEAddress pAddress);
    static void testNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                   uint8_t* pData, size_t length, bool isNotify);
    static void initBLEService();
    static void checkCharacterisitc();
    static void restartScan();

    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };
};

#endif



   