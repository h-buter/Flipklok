#include "stepperAdvance.h"
#include "timeKeeping.h"

/// Advances the stepper each time it gets called by one full step gets triggered by ISR_TA0().
/// Step sequence:
///  - 0001
///  - 0010
///  - 0100
///  - 1000
void stepperAdvance()
{
    static unsigned int coil;
    static unsigned int stepsTillOneSecond;

    P1OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4);
    P1OUT |= (1 << (coil + 2));
    coil++;
    if (coil == 5)
    {
        coil = 1;
    }

    stepsTillOneSecond++;
    if (stepsTillOneSecond >= 2048.0 * 1.6)
    timeMechanical ;

}
