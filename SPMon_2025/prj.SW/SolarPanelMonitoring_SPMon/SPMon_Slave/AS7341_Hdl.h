#ifndef AS7341_HDL_H
#define AS7341_HDL_H

#include <Arduino.h>
#include <Adafruit_AS7341.h>

/* AS7341 sensor object (singleton) */
extern Adafruit_AS7341 as7341;

/* Spectral components idx */
#define IDX_VIOLET        0
#define IDX_WHITE         1
#define IDX_WHITE_GREEN   2
#define IDX_GREEN         3
#define IDX_YELLOW_GREEN  4
#define IDX_YELLOW        5
#define IDX_ORANGE_RED    6
#define IDX_RED           7
#define IDX_NIR           8
#define IDX_CLEAR         9

#define NUM_CHANNELS      10

/* Spectral component array */
extern volatile uint16_t violet, white, whiteGreen, green, yellowGreen, yellow, orangeRed, red, nearInfrared, clear;
extern volatile uint16_t* AS7341_spectralArray[NUM_CHANNELS];

void AS7341_init();
void AS7341_getSpectralComponents(uint16_t* spectralArray);

#endif /* AS7341_HDL_H */
