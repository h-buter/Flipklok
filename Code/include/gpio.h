/*
 * gpio.h
 *
 *  Created on: 20 mrt. 2025
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <msp430.h>
#include <pins.h>

void setupGpio();
void setupClockIO();
void wakeUp();
void sleep();


#endif /* GPIO_H_ */
