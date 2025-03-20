/*
 * gpio.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "gpio.h"

void setupGpio()
{
    // Configure GPIO
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
    P1OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4); // Clear stepper pins output latch for a defined power-on state
    P1DIR |= STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4; // Set stepper pins to output direction
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    PJSEL0 |= BIT4 | BIT5;                  // Set PJSEL0 BIT4 to high = LFXIN Crystal mode,  and BIT5 to high = IO on this pin disabled


    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
}
