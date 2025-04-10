/*
 * adc.h
 *
 *  Created on: 21 mrt. 2025
 */

#ifndef ADC_H_
#define ADC_H_
#include <msp430.h>

void setupAdc();
void startAdcConv();
double convertAdc2Voltage(int R1, int R2, int adcReading);

#define NUM_CHANNELS 3
#define referenceVoltage 2.5
#define adcMax 1023

volatile unsigned int adcResults[NUM_CHANNELS];



#endif /* ADC_H_ */
