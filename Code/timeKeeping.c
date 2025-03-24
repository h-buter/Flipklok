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
    unsigned int state = __get_SR_register() & GIE; __disable_interrupt();  // Save current interrupt state and disable interrupts
    int timeDiff = timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage - (int)timeMechanical;
    if (state) __enable_interrupt(); // Restore previous interrupt state
    stepsRemaining = calculateStepsToTake(timeDiff);

    __no_operation();

    if (0 != stepsRemaining) // difference was large enough to activate stepper
    {
        toggleCalculateTimeDifference = 0;  // disable triggering time difference function in timer interrupt (this function)
        TA0CCTL2 |= CCIE;                   // TACCR2 interrupt stepperAdvance enable
    }
}

double steps;

int calculateStepsToTake(int timeDiff)
{
    if (timeDiff < 0) //time difference is negative
    {
        __no_operation();
    }
    if (timeDiff >= stepsToggleThreshold) //Enough steps to toggle stepper
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

void resetTimeKeeping()
{
    timeMechanical = 0.0;
    timeOfLastDcfMessage = 0;
    timeSinceLastCompleteDcfMessage = 0;
}
