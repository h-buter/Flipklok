/*
 * stepperAdvance.h
 *
 */



#ifndef STEPPERADVANCE_H_
#define STEPPERADVANCE_H_
#include <msp430.h>
#include <pins.h>

#define stepperTimeOffset 0x600

void setupTimer0();
void stepperAdvance();

#endif /* STEPPERADVANCE_H_ */
