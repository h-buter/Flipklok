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
bool countingDirectionStart;
unsigned int endTime;
bool countingDirectionEnd;
bool currentBit;
#define bitArrayLength 10
bool bitArray[60];


#define bitTime0Sec 0.1
#define bitTime1Sec 0.2
#define bitTimeStartSec 1.8
#define  bitTimeCyclesTresh 0.01

//Correct casting
//0
#define  bitTimeCycles0 ((clockFreq) * (bitTime0Sec))
#define  bitTimeCycles0Upper ((int)((bitTimeCycles0) + ((clockFreq) * (bitTimeCyclesTresh))))
#define  bitTimeCycles0Lower ((int)((bitTimeCycles0) - ((clockFreq) * (bitTimeCyclesTresh))))
//1
#define  bitTimeCycles1 ((clockFreq) * (bitTime1Sec))
#define  bitTimeCycles1Upper ((int)((bitTimeCycles1) + ((clockFreq) * (bitTimeCyclesTresh))))
#define  bitTimeCycles1Lower ((int)((bitTimeCycles1) - ((clockFreq) * (bitTimeCyclesTresh))))
//start of DCF77 message
#define  bitTimeCyclesStart ((clockFreq) * (bitTimeStartSec))
#define  bitTimeCyclesStartUpper ((int)((bitTimeCyclesStart) + ((clockFreq) * (bitTimeCyclesTresh))))
#define  bitTimeCyclesStartLower ((int)((bitTimeCyclesStart) - ((clockFreq) * (bitTimeCyclesTresh))))


//bitTimeCycles0 = 3276.8
//bitTimeCycles0Upper = 3604.5
//bitTimeCycles0Lower = 2949.1
//bitTimeCycles1 = 6553.6
//bitTimeCycles1Upper = 6881.3
//bitTimeCycles1Lower = 6225.9


unsigned int calculateBitTransmitTime(bool checkSyncStatus, unsigned int start, bool startDir, unsigned int stop, bool stopDir);
bool bitValue(unsigned int bitTime);
void storeBit(bool bit, unsigned int location);
void decodeBitStream();

#endif /* DCFRECEIVE_H_ */
