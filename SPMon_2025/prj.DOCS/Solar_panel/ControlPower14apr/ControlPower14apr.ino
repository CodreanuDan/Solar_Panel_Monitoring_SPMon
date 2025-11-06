#include <Wire.h>
#include <INA226_WE.h>   
#define I2C_ADDRESS 0x40
 
INA226_WE ina226 = INA226_WE(I2C_ADDRESS);
// Define PWM output pin for L298 (adjust if needed)
const int pwmPin = 9;

// MPPT parameters
int pwmValue = 128;       // Initial PWM duty cycle (start in the middle)
int pwmStep = 2;         // Step size for PWM adjustment
unsigned long delayTime = 1000; // Delay between MPPT steps (milliseconds)

// Variables for P&O algorithm
float previousPower = 0;
int adjustmentDirection = 1; // 1 for increasing PWM, -1 for decreasing PWM

float shuntVoltage_mV = 0.0;
float loadVoltage_V = 0.0;
float busVoltage_V = 0.0;
float current_mA = 0.0;
float current_power_mW = 0.0;
int i=0;
 
void setup() 
{
  Serial.begin(9600);
  // while (!Serial); // wait until serial comes up on Arduino Leonardo or MKR WiFi 1010
  Wire.begin();
  ina226.init();
  pinMode(LED_BUILTIN, OUTPUT);
 
  /* Set Number of measurements for shunt and bus voltage which shall be averaged
    Mode *     * Number of samples
    AVERAGE_1            1 (default)
    AVERAGE_4            4
    AVERAGE_16          16
    AVERAGE_64          64
    AVERAGE_128        128
    AVERAGE_256        256
    AVERAGE_512        512
    AVERAGE_1024      1024*/
 
  ina226.setAverage(AVERAGE_64); // choose mode and uncomment for change of default
 
  /* Set conversion time in microseconds
     One set of shunt and bus voltage conversion will take:
     number of samples to be averaged x conversion time x 2
 
       Mode *         * conversion time
     CONV_TIME_140          140 µs
     CONV_TIME_204          204 µs
     CONV_TIME_332          332 µs
     CONV_TIME_588          588 µs
     CONV_TIME_1100         1.1 ms (default)
     CONV_TIME_2116       2.116 ms
     CONV_TIME_4156       4.156 ms
     CONV_TIME_8244       8.244 ms  */
 
  //ina226.setConversionTime(CONV_TIME_1100); //choose conversion time and uncomment for change of default
 
  /* Set measure mode
    POWER_DOWN - INA226 switched off
    TRIGGERED  - measurement on demand
    CONTINUOUS  - continuous measurements (default)*/
 
  //ina226.setMeasureMode(CONTINUOUS); // choose mode and uncomment for change of default
 
  /* Set Resistor and Current Range
     if resistor is 5.0 mOhm, current range is up to 10.0 A
     default is 100 mOhm and about 1.3 A*/
 
  ina226.setResistorRange(0.1, 1.3); // choose resistor 0.1 Ohm and gain range up to 1.3A
 
  /* If the current values delivered by the INA226 differ by a constant factor
     from values obtained with calibrated equipment you can define a correction factor.
     Correction factor = current delivered from calibrated equipment / current delivered by INA226*/
 
  ina226.setCorrectionFactor(0.93);
 
 // Serial.println("INA226 Current Sensor Example Sketch - Continuous");
 
  ina226.waitUntilConversionCompleted(); //if you comment this line the first data might be zero

  // Set PWM pin as output
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, pwmValue); // Set initial PWM
}
 
void loop()
{ 
  ina226.readAndClearFlags();
  shuntVoltage_mV = ina226.getShuntVoltage_mV();
  busVoltage_V = ina226.getBusVoltage_V();
  current_mA = ina226.getCurrent_mA();
  current_power_mW = ina226.getBusPower();
  loadVoltage_V  = busVoltage_V + (shuntVoltage_mV / 1000);
 
  /*Serial.print("Shunt Voltage [mV]: "); Serial.println(shuntVoltage_mV);
  Serial.print("Bus Voltage [V]: "); Serial.println(busVoltage_V);
  Serial.print("Load Voltage [V]: "); Serial.println(loadVoltage_V);
  Serial.print("Current[mA]: "); Serial.println(current_mA);
  Serial.print("Bus Power [mW]: "); Serial.println(current_power_mW);
  Serial.print("PWM Value: ");
  Serial.println(pwmValue);*/

  /*if (!ina226.overflow)
  {
    Serial.println("Values OK - no overflow");
  }
  else
  {
    Serial.println("Overflow! Choose higher current range");
  }
  Serial.println();*/
 
  // Perturb and Observe Algorithm
  if (current_power_mW > previousPower) {
    // Power increased, continue in the same direction
  } else {
    // Power decreased (or stayed the same), reverse the direction
    adjustmentDirection *= -1;
  }

  // Adjust PWM value
  pwmValue += (pwmStep * adjustmentDirection);

  // Limit PWM value to the valid range (0-255)
  pwmValue = constrain(pwmValue, 0, 255);

  // Output PWM signal
  analogWrite(pwmPin, pwmValue);

  // Update previous power
  previousPower = current_power_mW;
  if (i==0)
  {digitalWrite(LED_BUILTIN, HIGH);
  i=1;}
  else
  {digitalWrite(LED_BUILTIN, LOW);
  i=0;}

  // Delay for the next iteration
  delay(delayTime);
}