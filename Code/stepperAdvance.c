#include "stepperAdvance.h"
#include "timeKeeping.h"

/// Advances the stepper each time it gets called by one full step gets triggered by ISR_TA0().
/// Step sequence:
///  - 0001
///  - 0010
///  - 0100
///  - 1000

int stepperArray[] = {BIT0, BIT3, BIT4, BIT5};
void stepperAdvance()
{
    static unsigned int coil;
    if (stepsRemaining > 0 || timerOffsetToggle == 1)
    {
        P2OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4);
        P2OUT |= stepperArray[coil];
        if (coil == 3)
        {
            P2OUT |= stepperArray[0];
        }
        else
        {
            P2OUT |= stepperArray[coil + 1];
        }

        coil++;
        if (coil == 4)
        {
            P2OUT |= stepperArray[0];
            coil = 0;
        }


        if (timerOffsetToggle == 0)
        {
            stepsRemaining--;
            int test = timePerStep;
            timeMechanical = timeMechanical + timePerStep; //Increment mechanical time keeping counter with 1 step time
            __no_operation();
        }
    }
    else
    {
        TA0CCTL2 &= ~CCIE;                          // TACCR2 interrupt stepperAdvance disable
        toggleCalculateTimeDifference = 1;          // enable triggering time difference function in timer interrupt.
    }
}
