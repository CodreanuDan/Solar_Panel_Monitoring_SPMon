// #include "dht11.h"

// SPMon_DHT_11_Sensor_Library::SPMon_DHT_11_Sensor_Library(int dht_11_pin) : _dht_11_pin(dht_11_pin)
// {
//     pinMode(_dht_11_pin, OUTPUT);
//     /* Wake up the sensor */
//     digitalWrite(_dht_11_pin, HIGH);
// }

// void SPMon_DHT_11_Sensor_Library::DHT_11_StartSignal()
// {
//     pinMode(_dht_11_pin, OUTPUT);
//     /* Send signal to the sensor */
//     digitalWrite(_dht_11_pin, LOW);
//     vTaskDelay(DHT_11_ALERT_SIG_DURATION / pdMsTicks);
//     /* Pull up signal and wait for the senor to respond */
//     digitalWrite(_dht_11_pin, HIGH);
//     delayMicroseconds(DHT_11_REQUEST_SIG_DURATION);
//     /* Change to input to recive sensor data */
//     pinMode(_dht_11_pin, INPUT);
// }

// int SPMon_DHT_11_Sensor_Library::DHT_11_ReadRawData(byte data[5])
// {
//     /* Send start signal */
//     DHT_11_StartSignal();
//     /* set timout srart counter*/
//     unsigned long timeout_start = millis();

//     /* Check timeout error */
//     while (digitalRead(_dht_11_pin) == HIGH)
//     {
//         if (millis() - timeout_start > DHT_11_TIMEOUT_DURATION)
//         {
//             return DHT_11_ERROR_TIMEOUT;
//         }
//     }

//     /* DHT 11 response routine */
//     if (digitalRead(_dht_11_pin) == LOW)
//     {
//         /* Wait for the sensor to respond ; dht pull down voltage 80 us*/
//         delayMicroseconds(DHT_11_RESPONSE_SIG_DURATION);
//         if (digitalRead(_dht_11_pin) == LOW)
//         {
//             /* Wait for the sensor to respond ; dht pulls up voltage 80 us*/
//             delayMicroseconds(DHT_11_RESPONSE_SIG_DURATION);
//             /* Read data bytes from the sensor (40 bit message)*/
//             for (int i = 0; i <= 5; i++)
//             {
//                 /* Append the data to the data array*/
//                 data[i] = DHT_11_ReadByte();
//                 if (data[i] == DHT_11_ERROR_TIMEOUT)
//                 {
//                     return DHT_11_ERROR_TIMEOUT;
//                 }
//             }

//             /* Check the CRC*/
//             if(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
//             {
//                 return 0;
//             }
//             else
//             {
//                 return DHT_11_ERROR_CRC;
//             }
//         }
//         else
//         {
//             return DHT_11_ERROR_TIMEOUT;
//         }
//     }
// }

// byte SPMon_DHT_11_Sensor_Library::DHT_11_ReadByte()
// {
//     byte value = 0;
//     for(int i = 0; i < 8; i++)
//     {
//         while(digitalRead(_dht_11_pin) == LOW);
//         delayMircoseconds(30);
//         /* Build the data byte */
//         if(digitalRead(_dht_11_pin) == HIGH)
//         {
//             /* Shift to the left, first bit is MSB */
//             value |= (1 << (7 - i));
//         }
//          while(digitalRead(_dht_11_pin) == HIGH);
//     }

//     return value;
// }

// int SPMon_DHT_11_Sensor_Library::DHT_11_ReadTemeperature(int &temperature)
// {
//     byte data[5];
//     int error = DHT_11_ReadRawData(data);
//     if(error != 0)
//     {
//         return error;
//     }
//     temperature =  data[2];
// }

// int SPMon_DHT_11_Sensor_Library::DHT_11_ReadHumidity(int &humidity)
// {
//     byte data[5];
//     int error = DHT_11_ReadRawData(data);
//     if(error != 0)
//     {
//         return error;
//     }
//     humidity =  data[2];
// }

// int SPMon_DHT_11_Sensor_Library::DHT_11_ReadAndCheckData()
// {
//     int dhtStatusTemp = DHT_11_ReadTemeperature(int &temperature)
//     int dhtStatusHum = DHT_11_ReadHumidity(int &humidity)
//     if( dhtStatusTemp != 0 && dhtStatusHum != 0)
//     {
//        temp_struct_pointer_placeholder = temperature;
//        hum_struct_pointer_placeholder = humidity;
//     }
//     else
//     {
//        temp_struct_pointer_placeholder = temperature;
//        temp_struct_pointer_placeholder  = humidity;
//     }
// }