/*
 * calculateTimeDifference.h
 *
 *  Created on: 21 mrt. 2025

 */

#ifndef CALCULATETIMEDIFFERENCE_H_
#define CALCULATETIMEDIFFERENCE_H_
#include <msp430.h>
#include <stdbool.h>

extern bool toggleCalculateTimeDifference;

void calculateTimeDifference();
int calculateStepsToTake();
void storeTimeLastCompleteDcfMessage();
void resetTimeKeeping();

unsigned int timeOfLastDcfMessage;
unsigned int timeSinceLastCompleteDcfMessage;
#define timerCountsInDay 60 * 60 * 24 / (capacityTimer / clockFreq)

#define toothGearStepper 15.0
#define toothGearClock 24.0
#define gearRatio toothGearClock / toothGearStepper
#define stepsFullRotationStepper 2048.0
#define stepsClockHour stepsFullRotationStepper * gearRatio
#define stepsClockMinute stepsClockHour / 60.0
#define stepsClockSecond stepsClockMinute / 60.0
#define timePerStep 3600.0 / (stepsFullRotationStepper * gearRatio)
#define stepsToggleThreshold 10

//Needs to be stored in non volatile storage (FRAM), to be persistent across boot
double timeMechanical; //time of mechanical clock mechanism

unsigned int stepsRemaining;


#endif /* CALCULATETIMEDIFFERENCE_H_ */
