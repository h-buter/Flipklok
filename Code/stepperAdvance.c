
#include "stepperAdvance.h"

// step sequence
//  0001
//  0010
//  0100
//  1000
void stepperAdvance()
{
    static unsigned int step;
    P1OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4);
    P1OUT |= (1 << step);
    step++;
    if (step == 7)
    {
        step = 3;
    }

}

//CCR0 uses TIMER0_A0_VECTOR rest of capture registers uses TIMER0_A1_VECTOR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void myISR_TA0_CCR0(void)
{

}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void myISR_TA0(void)
{
    switch(__even_in_range(TA0IV, 0x14 ))
    {
        case 0x00: break; // None
        case 0x02: break; // CCR1 IFG
        case 0x04:
                stepperAdvance(); // Call stepper function
                unsigned int newTime = TA0R + stepperTimeOffset;
                TA0CCR2 = newTime;
            break; // CCR2 IFG
        case 0x06: break; // CCR3 IFG
        case 0x08: break; // CCR4 IFG
        case 0x0A: break; // CCR5 IFG
        case 0x0C: break; // CCR6 IFG
        case 0x0E: break; // TA0IFG
        default: _never_executed();
    }
}
