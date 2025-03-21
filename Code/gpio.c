/*
 * gpio.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "gpio.h"

/// Setup of all GPIO's on startup
void setupGpio()
{

    P1OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4); // Clear stepper pins output latch for a defined power-on state
    P1DIR |= STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4; // Set stepper pins to output direction
    PJSEL0 |= BIT4 | BIT5;                  // Set PJSEL0 BIT4 to high = LFXIN Crystal mode,  and BIT5 to high = IO on this pin disabled

    //fwdButton
    P1DIR &= ~BIT1;                             // Input
    P1REN |= BIT1;                              // Enable pull
    P1OUT |= BIT1;                              // Enable pull up
    P1IES |= BIT1;                              // High to low
    P1IFG &= ~BIT1;                             // Clear flag
    P1IE |= BIT1;                               // Enable interrupt

    P1OUT &= ~BIT0;                             //Turn off LED0
    P1DIR |= BIT0;                              //LED0 as output

    //led for lightning the clock
    P4OUT &= ~BIT6;                             //Turn off LED0
    P4DIR |= BIT6;                              //LED0 as output


    //dcf button for testing
    P4DIR &= ~BIT5;                             // Input
    P4REN |= BIT5;                              // Enable pull
    P4OUT |= BIT5;                              // Enable pull up
    P4IES |= BIT5;                              // High to low
    P4IFG &= ~BIT5;                             // Clear flag
    P4IE |= BIT5;                               // Enable interrupt





    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}
