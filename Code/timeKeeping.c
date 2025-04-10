/*
 * timekeeping.c
 *
 *  Created on: 21 mrt. 2025
 */
/**
 * @file timeKeeping.c
 * @brief handles the keeping and calculating of time of the system
 */

#include "timeKeeping.h"
#include "dcfReceive.h"
#include "uart.h"
#include "pinInterrupts.h"

bool toggleCalculateTimeDifference = 1;

//declare it globally
uint32_t timeOfLastDcfMessage = 0;
uint32_t timeSinceLastCompleteDcfMessage = 0;
double mechanicalTimeFloat = 0;


/**
 * @brief calculate the difference between the mechanical time and the current time to have the time difference which the stepper needs to be advanced with, gets called from end interrupt of ISR_TA0(void).
 *
 * It then calculates the amount of steps that are needed to advance the clock to display the current time.
 * The function take in to account which way it is the fastest to fully wrap the clock or to wait until the time synchronizes
 */

void calculateTimeDifference()
{
    if(countDcf77Messages > 0)
    {
        __no_operation();
        volatile int testing = 0;
    }
    volatile uint32_t timeDiff;
    __no_operation();
    toggleInterruptDcf = 1; // Enable dcfReceive interrupt
//    unsigned int state = __get_SR_register() & GIE; __disable_interrupt();  // Save current interrupt state and disable interrupts
    if (0 == toggleFwdInterrupt) //fwdButton is not pressed otherwise skip time keeping
    {
        if(0 == countDcf77Messages) // When no DCF77 message has been received work only on internal clock
        {
            timeDiff = timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage - mechanicalTimeFloat;
            P1DIR |= BIT7;                              // set as output enables indicator LED, indicating clock is off time
        }
        else if(countDcf77Messages > 0) // One or more DCF77 messages has been received use the time of DCF to set clock
        {
            if((timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage) > mechanicalTimeFloat) // Clock runs behind need to be advanced to current time
            {
                timeDiff = (timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage) - mechanicalTimeFloat; // Calculate how much the clock runs behind
            }
            else if((timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage) < mechanicalTimeFloat) //Clock runs ahead
            {
                if((mechanicalTimeFloat - (timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage)) > clockWraparoundThresholdSec) // When the time that the clock runs ahead is greater than the threshold than rotate to 00:00 so it can sync from there
                {
                    timeDiff = secondsInDay + 60 - mechanicalTimeFloat; // Calculate the amount of time between clock current state and the time until 00:01
                }
                else if((mechanicalTimeFloat - (timeOfLastDcfMessage + timeSinceLastCompleteDcfMessage)) <= clockWraparoundThresholdSec) // If the time that the clock runs ahead is less or equal to the threshold it is faster to just wait until the clock is on time
                {
                    timeDiff = 0;
                }
                else
                {
                    __no_operation(); //?
                }
            }
            else
            {
                __no_operation(); //?
            }
        }
        else
        {
            __no_operation(); //?
        }

        //Checking if calculated time is plausible
        if (timeDiff > secondsInDay)
        {
            __no_operation(); //Clock will rotate more than a full turn, disable stepper and recalculate
        }
        else // Calculated time is plausible convert to needed steps
        {
            if (timeDiff > 60) // If the time difference is large enough stop receiving dcf messages to prevent crosstalk between stepper and receiver, is enabled again at start of this function
            {
                toggleInterruptDcf = 0; // Disable dcfReceive interrupt until clock is done syncing
                P1DIR |= BIT7;                              // set as output enables indicator LED, indicating clock is off time
                toggleFwdInterruptISR = 0; // Disable fwdButton when calculating time and adjusting the clock (enabled again in stepperAdvance function)
                timerCompareStepperSpeedToggle = 1; //set stepper speed to fast
            }
            else
            {
                timerCompareStepperSpeedToggle = 0; //set stepper speed to slow
                if (0 < countDcf77Messages)
                {
                    P1DIR &= ~BIT7;                              // set as input disables indicator LED, indicating clock runs on time
                }
            }
            stepsRemaining = calculateStepsToTake(timeDiff);
//            #ifdef UART_ENABLED
//                UART_SendInt(timeDiff);
//            #endif

            if (stepsRemaining >= stepsToggleThreshold && stepsRemaining < stepsInDay) //Check if the needed steps to take is larger than the movement threshold, this prevents excessive use of the motor
            {
                P1OUT |= BIT0;      //Turn 5v on
                toggleCalculateTimeDifference = 0;  // Disable triggering time difference function in timer interrupt (this function), will be enabled again in stepperAdvance function
                TA0CCTL2 |= CCIE;                   // TACCR2 interrupt stepperAdvance enable
            }
        }
    }

//    if (state) __enable_interrupt(); // Restore previous interrupt state
}



/**
 * @brief Calculate the steps to take for a given time difference, gets called by calculateTimeDifference()
 *
 * @param time the difference between time to advance the stepper with
 * @return uint32_t amount of steps that the stepper needs to be rotated with
 */
uint32_t calculateStepsToTake(uint32_t time)
{
    if (time <= 0) //time difference is negative or 0
    {
        __no_operation();
        return 0;
    }
    else
    {
        double steps = (double)time * stepsClockSecond;
        uint32_t stepsRounded = (uint32_t)steps;
        __no_operation();
        return stepsRounded;
    }
}

/**
 * @brief Reset the time settings on startup, gets called from main()
 *
 */
void resetTimeKeeping()
{
    mechanicalTimeFloat = 0.0;
    timeOfLastDcfMessage = 0;
    timeSinceLastCompleteDcfMessage = 0;
}
