/*
 * calculateTimeDifference.h
 *
 *  Created on: 21 mrt. 2025

 */

#ifndef CALCULATETIMEDIFFERENCE_H_
#define CALCULATETIMEDIFFERENCE_H_
#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>  // Include to define uint32_t

extern bool toggleCalculateTimeDifference;

void calculateTimeDifference();
uint32_t calculateStepsToTake();
void storeTimeLastCompleteDcfMessage();
void resetTimeKeeping();

extern uint32_t timeOfLastDcfMessage;
extern uint32_t timeSinceLastCompleteDcfMessage;
#define timerCountsInDay 60 * 60 * 24 / (capacityTimer / clockFreq)

#define toothGearStepper 15.0
#define toothGearClock 24.0
#define gearRatio (toothGearClock / toothGearStepper)
#define stepsFullRotationStepper 2048.0
#define stepsClockHour (stepsFullRotationStepper * gearRatio)
#define stepsClockMinute (stepsClockHour / 60.0)
#define stepsClockSecond (stepsClockMinute / 60.0)
#define timePerStep (3600.0 / (stepsFullRotationStepper * gearRatio))
#define stepsToggleThreshold 20
#define clockWraparoundThresholdSec (10 * 60)
#define stepsWrap (stepsClockHour * 24)
#define secondsInDay 86400
#define stepsInDay (stepsClockSecond * secondsInDay)

//Needs to be stored in non volatile storage (FRAM), to be persistent across boot
extern double mechanicalTimeFloat; //time of mechanical clock mechanism should be in double/floating point
//uint32_t mechanicalTimeInt; //Integer display of floatingpoint timeMechanical

uint32_t stepsRemaining;


volatile bool toggleTimer1Direction; // 1 is up 0 is down


#endif /* CALCULATETIMEDIFFERENCE_H_ */
