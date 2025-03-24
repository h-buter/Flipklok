/*
 * timer.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "led.h"
#include "timer.h"
#include "adc.h"
#include "timeKeeping.h"


//------------------------------------------Timer0---------------------------------------------------

/// Setup of timer0, uses the the ACLK (Auxiliary clock), counts up continuous.
/// Capture control registers
///     - CCR0
///     - CCR1 triggers pwm LED
///     - CCR2 triggers stepperAdvance()
///     - TAIFG start reading ADC
void setupTimer0()
{
    //Timer setup
    TA0CTL = TASSEL__ACLK | MC__CONTINUOUS| TACLR | TAIE | ID__1;    // Set to counter to ACLK, Continuous clock, clear counter of clock, Enable TAIFG interrupt, Divider set to 1
    TA0CCTL1 = CCIE;                // TACCR1 interrupt enabled
    TA0CCTL2 &= ~CCIE;                // TACCR2 interrupt disable
    TA0CCR2 = stepperTimeOffsetSlow;             // compare at stepperOffset
    TA0CCR1 = pwmPeriod_cycles;
}

/// Interrupt vector of timer0 CCR0
// CCR0 uses TIMER0_A0_VECTOR rest of capture registers uses TIMER0_A1_VECTOR
#pragma vector=TIMER0_A0_VECTOR
__interrupt void ISR_TA0_CCR0(void)
{
    TA0CCTL0 &= ~CCIFG; // Clear CCR0 interrupt flag
    //Not used yet
}

/// Interrupt vector of timer0 CCR1 till CCR2 and TA0IFG
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
            break; // CCR1 IFG used for PWM LED
        case 0x04:
                if (timerOffsetToggle == 0)     //calculate new normal timer capture
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetSlow;
                }
                else                            //calculate new fast timer capture
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetFast;
                }
                stepperAdvance(); // Call stepper function
            break; // CCR2 IFG stepper
        case 0x0E:
            startAdcConv();
            if (toggleCalculateTimeDifference == 1 && timerOffsetToggle == 0)
            {
                calculateTimeDifference();
            }
            break; // TA0IFG
        default: _never_executed();
    }
}

//------------------------------------------Timer1---------------------------------------------------

/// Setup of timer1, uses the the ACLK (Auxiliary clock), counts up/down.
/// Capture control registers
///     - CCR0
///     - CCR1
///     - CCR2
///     - TAIFG trigger counting timer1Count up
void setupTimer1()
{
    //Timer setup
    TA1CTL = TASSEL__ACLK | MC__UPDOWN | TACLR | ID__1;     // Set to counter to ACLK, Continuous clock, clear counter of clock, Enable TAIFG interrupt, Divider set to 1
    TA1CCR0 = capacityTimer - 1;                            // Every 4 seconds
    TA1CCTL0 = CCIE;                                        // TACCR0 interrupt enabled
//    TA1CCTL1 = CCIE;                                      // TACCR1 interrupt enabled
//    TA1CCTL2 = CCIE;                                      // TACCR2 interrupt enabled
    timeSinceLastCompleteDcfMessage = 0;
}



/// Interrupt vector of timer1 CCR0
// CCR0 uses TIMER1_A0_VECTOR rest of capture registers uses TIMER1_A1_VECTOR
#pragma vector=TIMER1_A0_VECTOR
__interrupt void ISR_TA1_CCR0(void)
{
    TA1CCTL0 &= ~CCIFG; // Clear own CCR0 interrupt flag
    timeSinceLastCompleteDcfMessage += 4; //Increment counter by 4 seconds
    if (timeSinceLastCompleteDcfMessage >= timerCountsInDay)
    {
        timeSinceLastCompleteDcfMessage = 0;
    }
}

/// Interrupt vector of timer1 CCR1 till CCR2 and TA0IFG
#pragma vector=TIMER1_A1_VECTOR
__interrupt void ISR_TA1(void)
{
    switch(__even_in_range(TA1IV, 0x0E ))
    {
        case 0x00: break; // None
        case 0x02: break; // CCR1 IFG
        case 0x04: break; // CCR2 IFG
        case 0x0E: break; // TA0IFG will never be reached in up/down mode
        default: _never_executed();
    }
}
