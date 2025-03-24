/*
 * dcfReceive.h
 *
 *  Created on: 21 mrt. 2025
 */

#ifndef DCFRECEIVE_H_
#define DCFRECEIVE_H_

#include <msp430.h>
#include <stdbool.h>
#include "timer.h"

bool interruptEdgeToggleDcf;
unsigned int startTime;
unsigned int endTime;
bool currentBit;
bool bitArray[60];


#define bitTime0Sec 0.1
#define bitTime1Sec 0.2
#define  bitTimeCyclesTresh 0.01

//Correct casting
#define  bitTimeCycles0 ((clockFreq) * (bitTime0Sec))
#define  bitTimeCycles0Upper ((int)((bitTimeCycles0) + ((clockFreq) * (bitTimeCyclesTresh))))
#define  bitTimeCycles0Lower ((int)((bitTimeCycles0) - ((clockFreq) * (bitTimeCyclesTresh))))

#define  bitTimeCycles1 ((clockFreq) * (bitTime1Sec))
#define  bitTimeCycles1Upper ((int)((bitTimeCycles1) + ((clockFreq) * (bitTimeCyclesTresh))))
#define  bitTimeCycles1Lower ((int)((bitTimeCycles1) - ((clockFreq) * (bitTimeCyclesTresh))))


unsigned int calculateBitTransmitTime(unsigned int start, unsigned int stop);
bool bitValue(unsigned int bitTime);

#endif /* DCFRECEIVE_H_ */
