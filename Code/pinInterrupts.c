/*
 * pinInterrupts.c
 *
 *  Created on: 27 mrt. 2025
 */
/**
 * @file pinInterrupts.c
 * @brief Setup and control of pin interrupts and location of the pin interrupt ISR's
 */

#include "pinInterrupts.h"
#include "dcfReceive.h"
#include "timeKeeping.h"


volatile unsigned int lastDirection = 0; // 0 = counting up, 1 = counting down

volatile int toggle123 = 0;

/**
 * @brief Interrupt vector of port1, gets called when a interrupt on this port is reached
 *
 * Used for the DCF77 pin where the receiver is connected to
 */
#pragma vector=PORT1_VECTOR
__interrupt void port1ISR(void)
{
//    if (P1IFG & BIT0)  // Check if interrupt occurred on P2.0
//    {
//        P1IFG &= ~BIT0;  // Clear interrupt flag
//    }
//    if (P1IFG & BIT1)  // P1.1
//    {
//        P1IFG &= ~BIT1;  // Clear interrupt flag
//    }
//    if (P1IFG & BIT2)  // P1.2 // start bit time measurement for DCF77 signals
//    {
//        P1IFG &= ~BIT2;                             // Clear flag
//    }
//    if (P1IFG & BIT3)  // P1.3
//    {
//        P1IFG &= ~BIT3;  // Clear interrupt flag
//    }
//    if (P1IFG & BIT4)  // P1.4
//    {
//        P1IFG &= ~BIT4;  // Clear interrupt flag
//    }
    if (P1IFG & BIT5)  // P1.5 DCF pin
    {
        P1IFG &= ~BIT5;  // Clear interrupt flag
        P1IE |= BIT5;    // Re-enable interrupt for P1.5
        __no_operation();
        if (0 == countDcf77Messages)
        {
            if (toggle123 == 1)
            {
                P1OUT |= BIT7; //Toggle indicator LED
                toggle123 = 0;
            }
            else
            {
                P1OUT &= ~BIT7; //Toggle indicator LED
                toggle123 = 1;
            }
        }
        if(1 == toggleInterruptDcf)
        {
            interruptDcf();
        }

        __no_operation();
    }
//    if (P1IFG & BIT6)  // P1.6
//    {
//        P1IFG &= ~BIT6;  // Clear interrupt flag
//    }
//    if (P1IFG & BIT7)  // P1.7
//    {
//        P1IFG &= ~BIT7;  // Clear interrupt flag
//    }

}

/**
 * @brief Interrupt vector of port2, gets called when a interrupt on this port is reached
 *
 * Used for the fwdButton to fast forward the mechanical time
 */
#pragma vector=PORT2_VECTOR
__interrupt void port2ISR(void)
{
    // Check if interrupt occurred
    if (P2IFG & BIT0)  // P2.0
    {
        P2IFG &= ~BIT0;  // Clear interrupt flag
    }
    if (P2IFG & BIT1)  // P2.1 fwdButton
    {
        P2IE &= ~BIT1;                               // disable interrupt
        if(1 == toggleFwdInterruptISR)
        {
            __no_operation();
            if (timerCompareStepperSpeedToggle == 0) // Toggle edge interrupt If rising edge was set, switch to falling & vice versa
            {
                P2IES |= BIT1;
                timerCompareStepperSpeedToggle = 1;
                toggleFwdInterrupt = 1;
                TA0CCTL2 |= CCIE;                // TACCR2 interrupt enable
            }
            else
            {
                P2IES &= ~BIT1;
                timerCompareStepperSpeedToggle = 0;
                toggleFwdInterrupt = 0;

            //                TA0CCTL2 &= ~CCIE;   not needed stepperAdvance enables calculateTime function automaticly when remaining steps are 0             // TACCR2 interrupt disable control back to calculateTimeDifference function
            }
        }
        else //turn speed toggle off when function is disabled
        {
            timerCompareStepperSpeedToggle = 0;
        }

        P2IFG &= ~BIT1;  // Clear interrupt flag
        P2IE |= BIT1;                               // enable interrupt
    }
//    if (P2IFG & BIT2)  // P2.2
//    {
//        P2IFG &= ~BIT2;  // Clear interrupt flag
//    }
//    if (P2IFG & BIT3)  // P2.3
//    {
//        P2IFG &= ~BIT3;  // Clear interrupt flag
//    }
//    if (P2IFG & BIT4)  // P2.4
//    {
//        P2IFG &= ~BIT4;  // Clear interrupt flag
//    }
//    if (P2IFG & BIT5)  // P2.5
//    {
//        P2IFG &= ~BIT5;  // Clear interrupt flag
//    }
//    if (P2IFG & BIT6)  // P2.6
//    {
//        P2IFG &= ~BIT6;  // Clear interrupt flag
//    }
//    if (P2IFG & BIT7)  // P2.7
//    {
//        P2IFG &= ~BIT7;  // Clear interrupt flag
//    }
}
