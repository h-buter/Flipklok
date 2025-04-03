/*
 * calculateTimeDifference.c
 *
 *  Created on: 21 mrt. 2025

 */

#include "timeKeeping.h"
#include "dcfReceive.h"
#include "uart.h"

bool toggleCalculateTimeDifference = 1;


uint32_t timeDiff;

bool testToggle = 0;

void calculateTimeDifference()
{
    static unsigned int uartDispCount = 5;
    if(testToggle == 1)
    {
        __no_operation();
    }
    unsigned int state = __get_SR_register() & GIE; __disable_interrupt();  // Save current interrupt state and disable interrupts
//    mechanicalTimeInt = (uint32_t)mechanicalTimeFloat; //convert from floating mechanical time to integer

    if ((timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage) < mechanicalTimeFloat) // Determine if time will be negative, so the mechanical time is after current time
    {
        if ((timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage + clockWraparoundThresholdSec) < mechanicalTimeFloat) //If the time difference is larger than the time it takes to wrap the clock fully around than wrap
        {
            uint32_t stepsToTake = calculateStepsToTake((mechanicalTimeFloat - timeOfLastDcfMessage - timeSinceLastCompleteDcfMessage));
            stepsRemaining = stepsWrap - stepsToTake;
//            UART_SendString("tDiff neg-large, steps: ");
//            UART_SendUint32(stepsRemaining);
//            UART_SendString(", mechTime: ");
//            UART_SendTime(mechanicalTimeFloat);
//            UART_SendString("\r\n");

            toggleInterruptDcf = 0; //disable interrupt DCF triggering dcf functions while the stepper is syncing

            testToggle = 1;
        }
        else // Time difference is negative and is less then threshold so it shorter to wait until it is in sync, so wait for new DCF message and let the clock stand still
        {
            timeDiff = 0;
            UART_SendString("tDiff neg-small, wait\r\n");
            TA0CCTL2 &= ~CCIE;                   // TACCR2 interrupt stepperAdvance disable, let clock wait
            toggleInterruptDcf = 1; //enable interrupt DCF triggering dcf functions
            if (state) __enable_interrupt(); // Restore previous interrupt state
            return; // Nothing further, so return and wait for next time check
        }
    }
    else // Time is not negative, so the mechanical time is before current time
    {
        timeDiff = timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage - mechanicalTimeFloat;
        stepsRemaining = calculateStepsToTake(timeDiff);
//        UART_SendString("tDiff pos: ");
//        UART_SendUint32(timeDiff);
//        UART_SendString(", steps: ");
//        UART_SendUint32(stepsRemaining);
//        UART_SendString(", mechTime: ");
//        UART_SendTime(mechanicalTimeFloat);
//        UART_SendString("\r\n");

        toggleInterruptDcf = 1; //enable interrupt DCF triggering dcf functions
    }

    if (state) __enable_interrupt(); // Restore previous interrupt state

    //Checking if timeDiff is plausible
    if (countDcf77Messages > 0) //more than 1 DCF77 message received so it is synced
    {

    }

    uartDispCount++;
    if (stepsRemaining >= stepsToggleThreshold && stepsRemaining <= stepsWrap) // Time difference was large enough to activate stepper and no to large that it is doing more than one rev
    {
        __no_operation();
        toggleCalculateTimeDifference = 0;  // Disable triggering time difference function in timer interrupt (this function), will be enabled again in stepperAdvance function
        TA0CCTL2 |= CCIE;                   // TACCR2 interrupt stepperAdvance enable
    }
    else if(stepsRemaining > stepsWrap && uartDispCount >= 5)
    {
        UART_SendString("S > wrap: ");
        UART_SendUint32(stepsRemaining);
        UART_SendString("\r\n");
        uartDispCount = 0;
    }
    return;
}

double steps;

uint32_t calculateStepsToTake(uint32_t timeDiff)
{
    if (timeDiff <= 0) //time difference is negative or 0
    {
        __no_operation();
        return 0;
    }
    else
    {
        steps = (double)timeDiff * stepsClockSecond;
        uint32_t stepsRounded = (uint32_t)steps;
        __no_operation();
        return stepsRounded;
    }
}

void resetTimeKeeping()
{
    mechanicalTimeFloat = 0.0;
    timeOfLastDcfMessage = 0;
    timeSinceLastCompleteDcfMessage = 0;
}
