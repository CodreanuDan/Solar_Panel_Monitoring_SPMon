#include <Arduino.h>
#include <SPIFFS.h>
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_core_dump.h"

#define CRASH_LOG_FILE "/crash_log.txt"
#define MAX_CRASH_LOGS 10

class CrashLogger {
private:
    static void logToSPIFFS(const char* message) {
        File file = SPIFFS.open(CRASH_LOG_FILE, FILE_APPEND);
        if (file) {
            file.println(message);
            file.close();
        }
    }

public:
    static void init() {
        if (!SPIFFS.begin(true)) {
            Serial.println("SPIFFS Mount Failed");
            return;
        }
        
        // Check if there was a crash on last boot
        checkForCrash();
    }

    static void checkForCrash() {
        esp_reset_reason_t reason = esp_reset_reason();
        
        File file = SPIFFS.open(CRASH_LOG_FILE, FILE_APPEND);
        if (!file) {
            Serial.println("Failed to open crash log file");
            return;
        }

        // Get timestamp
        char timestamp[64];
        snprintf(timestamp, sizeof(timestamp), "\n=== BOOT LOG [Uptime: %lu ms] ===\n", millis());
        file.print(timestamp);

        // Log reset reason
        file.print("Reset Reason: ");
        switch (reason) {
            case ESP_RST_UNKNOWN:
                file.println("Unknown");
                break;
            case ESP_RST_POWERON:
                file.println("Power-on");
                break;
            case ESP_RST_EXT:
                file.println("External pin");
                break;
            case ESP_RST_SW:
                file.println("Software reset");
                break;
            case ESP_RST_PANIC:
                file.println("*** PANIC/EXCEPTION ***");
                logPanicInfo(file);
                break;
            case ESP_RST_INT_WDT:
                file.println("*** INTERRUPT WATCHDOG ***");
                break;
            case ESP_RST_TASK_WDT:
                file.println("*** TASK WATCHDOG ***");
                break;
            case ESP_RST_WDT:
                file.println("*** OTHER WATCHDOG ***");
                break;
            case ESP_RST_DEEPSLEEP:
                file.println("Deep sleep");
                break;
            case ESP_RST_BROWNOUT:
                file.println("*** BROWNOUT ***");
                break;
            case ESP_RST_SDIO:
                file.println("SDIO reset");
                break;
            default:
                file.printf("Code: %d\n", reason);
                break;
        }

        file.close();
        
        // Print to serial for debugging
        Serial.printf("Reset reason: %d\n", reason);
        if (reason == ESP_RST_PANIC || reason == ESP_RST_INT_WDT || 
            reason == ESP_RST_TASK_WDT || reason == ESP_RST_WDT) {
            Serial.println("*** CRASH DETECTED - Check " CRASH_LOG_FILE " ***");
        }
    }

    static void logPanicInfo(File &file) {
        // Log additional panic information
        file.println("Panic details logged at boot");
        
        // You can add more detailed panic analysis here
        // The backtrace is usually printed to Serial by the ESP32 bootloader
    }

    static void readCrashLog() {
        if (!SPIFFS.exists(CRASH_LOG_FILE)) {
            Serial.println("No crash log found");
            return;
        }

        File file = SPIFFS.open(CRASH_LOG_FILE, FILE_READ);
        if (!file) {
            Serial.println("Failed to open crash log");
            return;
        }

        Serial.println("\n========== CRASH LOG ==========");
        while (file.available()) {
            Serial.write(file.read());
        }
        Serial.println("================================\n");
        file.close();
    }

    static void clearCrashLog() {
        if (SPIFFS.exists(CRASH_LOG_FILE)) {
            SPIFFS.remove(CRASH_LOG_FILE);
            Serial.println("Crash log cleared");
        }
    }

    static void logCustomMessage(const char* message) {
        File file = SPIFFS.open(CRASH_LOG_FILE, FILE_APPEND);
        if (file) {
            char timestamp[64];
            snprintf(timestamp, sizeof(timestamp), "[%lu ms] ", millis());
            file.print(timestamp);
            file.println(message);
            file.close();
        }
    }
};

// Example usage in your main sketch:
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("Starting...");
    
    // Initialize crash logger
    CrashLogger::init();
    
    // Read previous crash logs
    CrashLogger::readCrashLog();
    
    // Optional: Clear old logs after reading
    // CrashLogger::clearCrashLog();
}

void loop() {
    // Your normal code here
    
    // You can log custom messages at critical points:
    // CrashLogger::logCustomMessage("Entering critical section");
    
    delay(1000);
}

// Example: Force a crash for testing (DO NOT USE IN PRODUCTION!)
void testCrash() {
    // int* p = nullptr;
    // *p = 42;  // This will cause a crash
}
