/*
 * led.h
 *
 *  Created on: 21 mrt. 2025
 */

#ifndef LED_H_
#define LED_H_
#include <msp430.h>

void setupLedAndLDR();
void changeLedBrightness();

extern unsigned int pwmOnCycles;

#endif /* LED_H_ */
