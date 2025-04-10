/*
 * timer.c
 *
 *  Created on: 20 mrt. 2025
 */
/**
 * @file timer.c
 * @brief setup the timers and handle the timer ISR's
 */

#include "led.h"
#include "timer.h"
#include "adc.h"
#include "timeKeeping.h"
#include "pinInterrupts.h"


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
    TA0CTL = TASSEL_1 | MC_2| TACLR | TAIE | ID_0;    // Set to counter to ACLK, Continuous clock, clear counter of clock, Enable TAIFG interrupt, Divider set to 1
    TA0CCTL1 = CCIE;                // TACCR1 interrupt enabled
    TA0CCTL2 &= ~CCIE;                // TACCR2 interrupt disable
    TA0CCR2 = stepperTimeOffsetSlow;             // compare at stepperOffset
    TA0CCR1 = pwmPeriod_cycles;
}

void sleepTimer0()
{
    TA0CTL = TASSEL_1 | MC_2 | TAIE | ID_3;    // Set to counter to ACLK, Continuous clock, Enable TAIFG interrupt, Divider set to 8
    TA0CCTL1 &= ~CCIE;                // TACCR1 interrupt disable
    TA0CCTL2 &= ~CCIE;                // TACCR2 interrupt disable
}

void wakeUpTimer0()
{
    TA0CTL = TASSEL_1 | MC_2 | TAIE | ID_0;    // Set to counter to ACLK, Continuous clock, Enable TAIFG interrupt, Divider set to 1
    TA0CCTL1 |= CCIE;                // TACCR1 interrupt enable
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
    static unsigned int indicatorCount;
    static unsigned int togglePwm;
    switch(__even_in_range(TA0IV, 0x14 ))
    {
        case 0x00: break; // None
        case 0x02: // CCR1 IFG used for PWM LED
            if (0 == togglePwm)
            {
                TA0CCR1 = TA0R + pwmOnCycles;
                P1OUT |= BIT6; //turn on
//                P1OUT |= BIT7;
                togglePwm = 1;
            }
            else
            {
                TA0CCR1 = TA0R + pwmPeriod_cycles;
                P1OUT &= ~BIT6; //turn off
//                P1OUT &= ~BIT7;
                togglePwm = 0;

            }
            break;
        case 0x04: // CCR2 IFG stepper
                if (timerCompareStepperSpeedToggle == 0)     //calculate next normal timer capture
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetSlow;
                }
                else                            //calculate next fast timer capture
                {
                    TA0CCR2 = TA0R + stepperTimeOffsetFast;
                }
                stepperAdvance(); // Call stepper function
            break;
        case 0x0E:
            startAdcConv();
            if (toggleCalculateTimeDifference == 1 && toggleFwdInterrupt == 0) // When the fwdButton is not pressed and there are no more steps to take to get to the previous calculated time calculate new time difference.
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
    TA1CTL |= MC_0;         // stop timer
    TA1CTL |= TAIE;         // setup end of cycle interrupt (up/down mode, so it get triggered when reaching 0)
    TA1CTL |= TASSEL_1;     // Set to counter to ACLK
    TA1CTL |= TACLR;        // Clear counter of clock
    #warning "When using PCB divide by 1. When using MSP-EXP430G2ET devboard  pins on devboard are not connected to LFXTAL internal clock should be used"
    TA1CTL |= ID_0;         // Divider set to 1

    TA1CCR0 |= (capacityTimer - 1);                         // Every 4 seconds
    TA1CCTL0 |= CCIE;                                       // TACCR0 interrupt enabled
//    TA1CCTL1 = CCIE;                                      // TACCR1 interrupt enabled
//    TA1CCTL2 = CCIE;                                      // TACCR2 interrupt enabled
    timeSinceLastCompleteDcfMessage = 0;
    toggleTimer1Direction = 1; // Counting up
    TA1CTL |= MC_3;                                         // up/down

}




/// Interrupt vector of timer1 CCR0
// CCR0 uses TIMER1_A0_VECTOR rest of capture registers uses TIMER1_A1_VECTOR
#pragma vector=TIMER1_A0_VECTOR
__interrupt void ISR_TA1_CCR0(void)
{
    TA1CCTL0 &= ~CCIFG; // Clear own CCR0 interrupt flag
    toggleTimer1Direction = 0; // counting down
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
        case 0x0E:
            toggleTimer1Direction = 1; // counting up
            break; // TA0IFG will be reached when counting down and reaching 0 in up/down mode
        default: _never_executed();
    }
}
