/*
 * led.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "led.h"
#include "timer.h"

//int getLDR2Int()
//{
//b
//}

unsigned int pwmOnCycles = 0;

void changeLedBrightness(volatile unsigned int adcReading)
{
    pwmOnCycles = (int)((float)adcReading / 255 * pwmPeriod_cycles);
}




