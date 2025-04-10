/*
 * led.c
 *
 *  Created on: 21 mrt. 2025
 */

/**
 * @file led.c
 * @brief Control the brightness of the LED's
 */

#include "led.h"
#include "timer.h"

unsigned int pwmOnCycles = 0;

/**
 * @brief change the brightness od the LED's based on the LDR ADC reading, gets called from ADC10_ISR(void)
 *
 * @param[in] adcReading The reading from the adc of the LDR
 */
void changeLedBrightness(volatile unsigned int adcReading)
{
    adcReading = adcReading / 32; // scale from 0-1023 to 0-31
    adcReading = 33 - adcReading; // Invert so 32 = 0 and 0 = 32, added one to avoid pwmOnCycles = 0
    pwmOnCycles = adcReading;
}
