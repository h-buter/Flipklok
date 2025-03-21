/*
 * timer.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "led.h"
#include "timer.h"
#include "adc.h"



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
    TA0CCTL1 = CCIE;                // TACCR1 interrupt enabled
    TA0CCTL2 = CCIE;              // TACCR2 interrupt enabled
    TA0CCR2 = stepperTimeOffsetSlow;             // compare at stepperOffset
    TA0CCR1 = pwmPeriod_cycles;
}


/// Interrupt vector of timer0 CCR0
// CCR0 uses TIMER0_A0_VECTOR rest of capture registers uses TIMER0_A1_VECTOR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_TA0_CCR0(void)
{
    //Not used yet
}

/// Interrupt vector of timer0 CCR1 till CCR6 and TA0IFG
#pragma vector=TIMER0_A1_VECTOR
__interrupt void ISR_TA0(void)
{
    static unsigned int togglePwm;
    switch(__even_in_range(TA0IV, 0x14 ))
    {
        case 0x00: break; // None
        case 0x02:
            if (togglePwm == 0)
            {
                TA0CCR1 = TA0R + pwmOnCycles;
                P4OUT |= BIT6; //turn on
                togglePwm = 1;
            }
            else
            {
                TA0CCR1 = TA0R + pwmPeriod_cycles;
                P4OUT &= ~BIT6; //turn off
                togglePwm = 0;
            }
            break; // CCR1 IFG
        case 0x04:
                stepperAdvance(); // Call stepper function
                if (timerOffsetToggle == 0)
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetSlow;
                }
                else
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetFast;
                }

            break; // CCR2 IFG
        case 0x06: break; // CCR3 IFG
        case 0x08: break; // CCR4 IFG
        case 0x0A: break; // CCR5 IFG
        case 0x0C: break; // CCR6 IFG
        case 0x0E:
            startAdcConv();
            bool toggleCalculateTime = 0; // need to be implemented
            if (toggleCalculateTime == 1)
            {
//                calculateTimeDifference();
            }
            break; // TA0IFG
        default: _never_executed();
    }
}
