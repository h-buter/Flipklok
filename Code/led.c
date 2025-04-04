/*
 * led.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "led.h"
#include "timer.h"

unsigned int pwmOnCycles = 0;

void changeLedBrightness(volatile unsigned int adcReading)
{
//    __no_operation();
    adcReading = adcReading / 32; // scale from 0-1023 to 0-31
    adcReading = 33 - adcReading; // Invert so 32 = 0 and 0 = 32, added one to avoid pwmOnCycles = 0
//    pwmOnCycles = (int)((float)adcReading / 255 * pwmPeriod_cycles);
    pwmOnCycles = adcReading;
}
