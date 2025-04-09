/*
 * timer.h
 *
 *  Created on: 20 mrt. 2025
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <msp430.h>
#include "stepperAdvance.h"
#include "adc.h"

#define clockFreq 32768
#define capacityTimer 65536
#define timerDiv 1
#define timerLength_ms (capacityTimer / timerLength_cycles * 1000)
#define pwmPeriod_cycles 32 //(about 1 ms)


void setupTimer0();
void setupTimer1();

void wakeUpTimer0();
void sleepTimer0();
#endif /* TIMER_H_ */
