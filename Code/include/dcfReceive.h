/*
 * dcfReceive.h
 *
 *  Created on: 21 mrt. 2025
 */

#ifndef DCFRECEIVE_H_
#define DCFRECEIVE_H_

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>
#include "timer.h"

bool interruptEdgeToggleDcf;
unsigned int startTime;
bool countingDirectionStart;
unsigned int endTime;
bool countingDirectionEnd;
bool currentBit;
#define bitArrayLength 58 // needs to be 58, but 10 for testing
bool bitArray[60];

bool toggleInterruptDcf;


#define bitTime0Sec 0.1 //normal 0.1 for testing 0.05
#define bitTime1Sec 0.2 //normal 0.2 for testing 0.1
#define bitTimeSyncSec 1.8
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
#define  bitTimeCyclesSync ((unsigned int)(clockFreq * bitTimeSyncSec))
#define  bitTimeCyclesSyncUpper ((unsigned int)(bitTimeCyclesSync + (clockFreq * bitTimeCyclesTresh)))
#define  bitTimeCyclesSyncLower ((unsigned int)(bitTimeCyclesSync - (clockFreq * bitTimeCyclesTresh)))

volatile unsigned int minuteDcfLast;
volatile unsigned int hourDcfLast;

uint32_t countDcf77Messages;


unsigned int calculateBitTransmitTime(bool checkSyncStatus, unsigned int start, bool startDir, unsigned int stop, bool stopDir);
bool bitValue(unsigned int bitTime);
void storeBit(bool bit, unsigned int location);
void decodeBitStream2Seconds();
bool checkBitStream();
void interruptDcf();

#endif /* DCFRECEIVE_H_ */
