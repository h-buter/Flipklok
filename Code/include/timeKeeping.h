/*
 * calculateTimeDifference.h
 *
 *  Created on: 21 mrt. 2025

 */

#ifndef CALCULATETIMEDIFFERENCE_H_
#define CALCULATETIMEDIFFERENCE_H_
#include <stdbool.h>

extern bool toggleCalculateTimeDifference;

void calculateTimeDifference();
int calculateStepsToTake();
void storeTimeLastCompleteDcfMessage();

unsigned int timeOfLastDcfMessage;
unsigned int timeSinceLastCompleteDcfMessage;
#define timerCountsInDay 60 * 60 * 24 / (capacityTimer / clockFreq)

#define toothGearStepper 15
#define toothGearClock 24
#define gearRatio toothGearClock / toothGearStepper
#define stepsFullRotationStepper 2048
#define stepsClockHour stepsFullRotationStepper * gearRatio
#define stepsClockMinute stepsClockHour / 60
#define stepsClockSecond stepsClockMinute / 60
#define timePerStep 3600 / (stepsFullRotationStepper * gearRatio)

//Needs to be stored in non volatile storage (FRAM), to be persistent across boot
unsigned int timeMechanical; //time of mechanical clock mechanism


#endif /* CALCULATETIMEDIFFERENCE_H_ */
