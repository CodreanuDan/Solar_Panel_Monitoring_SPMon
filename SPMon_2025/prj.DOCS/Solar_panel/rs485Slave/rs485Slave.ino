#include <DHT.h>
#include <avr/sleep.h> // For sleep mode functions (for Arduino Leonardo)

// Define RS485 pins
const int RS485_RE_PIN = 8; 
const int RS485_DIR_PIN = 9;

// Define DHT22 sensor pin
#define DHTPIN 2 
#define DHTTYPE DHT22   

// HardwareSerial for RS485 communication
HardwareSerial rs485(1); 

// DHT sensor object
DHT dht(DHTPIN, DHTTYPE);

const char interrogationWord[] = "WAKEUP"; 

void setup() {
  rs485.begin(9600);

  pinMode(RS485_RE_PIN, INPUT);
  pinMode(RS485_DIR_PIN, INPUT);

  // Initialize DHT sensor
  dht.begin();

  // Enable serial interrupt
  rs485.enableReceiveInterrupts();

  // Enter sleep mode initially
  enterSleepMode(); 
}

void loop() {
  // This loop will not be executed while in sleep mode
}

// Interrupt Service Routine (ISR) for serial receive
void serialEvent() {
  // Clear any pending interrupts
  rs485.clearReceiveInterrupt();

  // Wake up from sleep
  wakeUpFromSleep(); 

  // Read the interrogation word (optional, for verification)
  char receivedWord[7]; 
  rs485.readBytes(receivedWord, 7); 
  if (strcmp(receivedWord, interrogationWord) == 0) {
    // Take measurement
    float temp = dht.readTemperature(); 
    float hum = dht.readHumidity();

    // Set RS485 direction to transmit
    digitalWrite(RS485_DIR_PIN, HIGH); 

    // Send temperature and humidity data
    rs485.print(temp); 
    rs485.print(",");
    rs485.println(hum); 
    delay(100); 

    // Set RS485 direction back to receive
    digitalWrite(RS485_DIR_PIN, LOW); 

    // Re-enter sleep mode
    enterSleepMode(); 
  }
}

// Function to enter sleep mode (for Arduino Leonardo)
void enterSleepMode() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  sleep_enable();
  attachInterrupt(0, wakeFromInterrupt, LOW); // Use interrupt 0 (digital pin 2) for wake-up
  sleep_mode(); 
  detachInterrupt(0); 
}

// Function to wake up from sleep
void wakeUpFromSleep() {
  sleep_disable(); 
}
