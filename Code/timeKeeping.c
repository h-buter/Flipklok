/*
 * calculateTimeDifference.c
 *
 *  Created on: 21 mrt. 2025

 */

#include "timeKeeping.h"

bool toggleCalculateTimeDifference = 1;

void calculateTimeDifference()
{
    __no_operation();
    int timeDiff = timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage - timeMechanical;
    int stepsReceived = calculateStepsToTake(timeDiff);

    __no_operation();

    if (0 == stepsReceived) // difference was too small to activate, stepper wait
    {

    }
}

double steps;

int calculateStepsToTake(int timeDiff)
{
    if (timeDiff < 0) //time difference is negative
    {
        __no_operation();
    }
    if (timeDiff >= 10) //toggle stepper and send steps
    {
        steps = (double)timeDiff * stepsClockSecond;
        int stepsRounded = (int)steps;
        __no_operation();
        return stepsRounded;

    }
    else // difference is to small to activate stepper
    {
        __no_operation();
        return 0;
    }
}
