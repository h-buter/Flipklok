/*
 * stepperAdvance.h
 *
 */



#ifndef STEPPERADVANCE_H_
#define STEPPERADVANCE_H_
#include <msp430.h>
#include "pins.h"
#include <stdbool.h>

#define stepperTimeOffsetSlow 0x800; //0x600 for testing setting faster
#define stepperTimeOffsetFast 0x100;

bool timerCompareStepperSpeedToggle;

void setupTimer0();
void stepperAdvance();

#endif /* STEPPERADVANCE_H_ */
