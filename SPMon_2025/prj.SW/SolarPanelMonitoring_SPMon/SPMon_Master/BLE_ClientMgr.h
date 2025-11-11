#ifndef BLE_CLIENT_MGR_H
#define BLE_CLIENT_MGR_H

#include "BLEDevice.h"
#include <BLEAdvertisedDevice.h>
#include "SensorPayload.h"

class BLE_ClientMgr
{
public:
    static void init();
    static void communicationManagerMainFunction();
    static const SensorPayload& getPayload() { return payload; }

private:
    /* BLE Server name (the other ESP32 name running the server sketch) - Copied from BLE_Client.ino */
    static constexpr const char* BLE_SERVER_NAME = "BLE_SERVER_ESP32";

    /* UUID's of the service, characteristic that we want to read - Copied from BLE_Client.ino */
    static inline BLEUUID serverServiceUUID = BLEUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
    static inline BLEUUID testCharacteristicUUID = BLEUUID("cba1d466-344c-4be3-ab3f-189f80dd7518");

    /* Global BLE objects and state */
    static inline SensorPayload payload;
    static inline BLERemoteCharacteristic* testCharacteristic = nullptr;
    static inline BLEAddress* pServerAddress = nullptr;
    static inline boolean doConnect = false;
    static inline boolean connected = false;

    /* Internal methods - based on static functions in BLE_Client.ino */
    static bool connectToServer(BLEAddress pAddress);
    static void testNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    static void initBLEService();
    static void checkCharacterisitc();
    static void restartScan();
    static void establishCommunication();

    /* Callback function that gets called, when another device's advertisement has been received */
    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };
};

#endif /* BLE_CLIENT_MGR_H */