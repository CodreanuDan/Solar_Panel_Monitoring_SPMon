#include "AS7341_Hdl.h"

/* Sensor object instance */ 
Adafruit_AS7341 as7341;

/* Spectral component array */
volatile uint16_t violet, white, whiteGreen, green, yellowGreen, yellow, orangeRed, red, nearInfrared, clear;
volatile uint16_t* AS7341_spectralArray[NUM_CHANNELS] = {
    &violet, &white, &whiteGreen, &green, &yellowGreen,
    &yellow, &orangeRed, &red, &nearInfrared, &clear
};

void AS7341_init()
{
    if (!as7341.begin())
    {
        Serial.println(">>> Could not find AS7341");
        // while (1);
    }
    as7341.setATIME(100);
    as7341.setASTEP(999);
    as7341.setGain(AS7341_GAIN_1X);
}

void AS7341_getSpectralComponents(uint16_t* spectralArray)
{
    uint16_t readings[NUM_CHANNELS];
    if (!as7341.readAllChannels(readings))
    {
        Serial.println(">>> Error reading all channels!");
        return;
    }
    else
    {
        spectralArray[IDX_VIOLET]       = readings[IDX_VIOLET];
        spectralArray[IDX_WHITE]        = readings[IDX_WHITE];
        spectralArray[IDX_WHITE_GREEN]  = readings[IDX_WHITE_GREEN];
        spectralArray[IDX_GREEN]        = readings[IDX_GREEN];
        spectralArray[IDX_YELLOW_GREEN] = readings[IDX_YELLOW_GREEN];
        spectralArray[IDX_YELLOW]       = readings[IDX_YELLOW];
        spectralArray[IDX_ORANGE_RED]   = readings[IDX_ORANGE_RED];
        spectralArray[IDX_RED]          = readings[IDX_RED];
        spectralArray[IDX_NIR]          = readings[IDX_NIR];
        spectralArray[IDX_CLEAR]        = readings[IDX_CLEAR];
    }
}

