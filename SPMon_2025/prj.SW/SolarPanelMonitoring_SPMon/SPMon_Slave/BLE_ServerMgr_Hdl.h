#ifndef BLE_SERVER_MGR_HDL
#define BLE_SERVER_MGR_HDL

/*
 * BLE Server for ESP32 - Send the payload from the sensors to the to client
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class BLE_ServerMgrHdl
{
    public:
        static void createServer();
        static void sendPayload(const uint8_t* data, size_t length);

    private:
        /* BLE Server name */
        static constexpr const char* BLE_SERVER_NAME = "BLE_SERVER_ESP32";
        /* BLE Servive UUID */
        static constexpr const char* SERVICE_UUID = "91bad492-b950-4226-aa2b-4ede9fa42f59";
        /* BLE Charactherisitc UUID */
        static constexpr const char* CHARACTERISTIC_UUID = "cba1d466-344c-4be3-ab3f-189f80dd7518";

        /* Global BLE objects */
        static inline BLEServer* pServer = nullptr;
        static inline BLECharacteristic* pCharacteristic = nullptr;
        static inline bool deviceConnected = false;

        /* Callbacks for onConnect and onDisconnect */
        class ServerCallbacks: public BLEServerCallbacks
        {
            void onConnect(BLEServer* pServer) override;
            void onDisconnect(BLEServer* pServer) override;
        };

};

#endif /* BLE_SERVER_MGR_HDL */

