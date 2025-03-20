/*
 * timer.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "timer.h"
#include "stepperAdvance.h"

/// Setup of timer0, uses the the ACLK (Auxiliary clock), counts up continuous.
/// Capture control registers
///     - CCR0
///     - CCR1
///     - CCR2 triggers stepperAdvance()
///     - TAIFG
void setupTimer0()
{
    //Timer setup
    TA0CTL = TASSEL__ACLK | MC__CONTINUOUS| TACLR | TAIE | ID__1;    // Set to counter to ACLK, Continuous clock, clear counter of clock, Enable TAIFG interrupt, Divider set to 1
    TA0CCTL2 = CCIE;              // TACCR2 interrupt enabled
    TA0CCR2 = stepperTimeOffsetSlow;             // compare at
}


/// Interrupt vector of timer0 CCR0
//CCR0 uses TIMER0_A0_VECTOR rest of capture registers uses TIMER0_A1_VECTOR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_TA0_CCR0(void)
{
    //Not used yet
}

/// Interrupt vector of timer0 CCR1 till CCR6 and TA0IFG
#pragma vector=TIMER0_A1_VECTOR
__interrupt void ISR_TA0(void)
{
    switch(__even_in_range(TA0IV, 0x14 ))
    {
        case 0x00: break; // None
        case 0x02: break; // CCR1 IFG
        case 0x04:
                stepperAdvance(); // Call stepper function
//                unsigned int newTime = 0;
                if (timerOffsetToggle == 0)
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetSlow;
                }
                else
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetFast;
                }

//                TA0CCR2 = newTime;
            break; // CCR2 IFG
        case 0x06: break; // CCR3 IFG
        case 0x08: break; // CCR4 IFG
        case 0x0A: break; // CCR5 IFG
        case 0x0C: break; // CCR6 IFG
        case 0x0E: break; // TA0IFG
        default: _never_executed();
    }
}
